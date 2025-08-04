/* MMA8451Q
 デフォルトではFullScaleが±2Gに設定されている。このとき、4096カウント/gであると、AN4076に記載がある
分解脳は14bitなので
12bit= 4096　-2048 〜 +2048
14bit=16384　-8192 〜 +8192 = -2g 〜 +2g

*/

#include "MMA8451Q_A.h"
#include <cstdint>

#define UINT14_MAX        16383

MMA8451Q_A::MMA8451Q_A(PinName sda, PinName scl, int addr) : m_i2c(sda, scl), m_addr(addr) {
    // activate the peripheral
    m_i2c.frequency(400000);
    uint8_t data[2] = {REG_CTRL_REG_1, 0x01};
    writeRegs(data, 2);
}

MMA8451Q_A::~MMA8451Q_A() { }

uint8_t MMA8451Q_A::setActive(){
    uint8_t dt[2];
    uint8_t data;
    readRegs(REG_CTRL_REG_1,&data,1);
    dt[0]=REG_CTRL_REG_1;
    dt[1]=data | 0x01;
    writeRegs(dt, 2);
    return( dt[1]);
}
uint8_t MMA8451Q_A::setStandby(){
    uint8_t dt[2];
    uint8_t data;
    readRegs(REG_CTRL_REG_1,&data,1);
    dt[0]=REG_CTRL_REG_1;
    dt[1]=data & 0xFE;
    writeRegs(dt, 2);
    return( dt[1]);
}
uint8_t MMA8451Q_A::getWhoAmI() {
    uint8_t who_am_i = 0;
    readRegs(REG_WHO_AM_I, &who_am_i, 1);
    return who_am_i;
}

uint8_t MMA8451Q_A::getTest(){
    uint8_t t[2];
    t[0]=REG_PULSE_CFG;
    t[1] = 0xFF;
    writeRegs(t,2);
    readRegs(REG_PULSE_CFG, t, 1);
    return(t[0]);
}

int16_t MMA8451Q_A::getAccX() {
    return (int16_t(getAccAxis(REG_OUT_X_MSB)));
}

int16_t MMA8451Q_A::getAccY() {
    return (int16_t(getAccAxis(REG_OUT_Y_MSB)));
}

int16_t MMA8451Q_A::getAccZ() {
    return (int16_t(getAccAxis(REG_OUT_Z_MSB)));
}

void MMA8451Q_A::getAccAllAxis(int16_t * res) {
    uint8_t val[6];// アドレス自動金玉を利用して全部読む
    int16_t xyz;
    readRegs(REG_OUT_X_MSB,val,6);
    for(int i=0;i<3;i++){
        xyz=(val[i*2] << 6) | (val[i*2+1] >> 2);    // to 14bit data
        if(xyz>UINT14_MAX/2){
            xyz -= UINT14_MAX;
        }
        res[i]=xyz;
    }
}

int16_t MMA8451Q_A::getAccAxis(uint8_t addr) {
    int16_t acc;
    uint8_t res[2];
    readRegs(addr, res, 2);

    acc = (res[0] << 6) | (res[1] >> 2);    // to 14bit data
    if (acc > UINT14_MAX/2)
        acc -= UINT14_MAX;

    return acc;
}
// rng : 2,4,8
// HPF : Enable high-pass output data 1 = output data high-pass filtered. Default value: 0.
float MMA8451Q_A::setRange(uint8_t rng,uint8_t hpf){
    uint8_t data[2];
    data[0] = REG_XYZ_DATA_CFG;
    if ( (rng<3)||(hpf==0)||(hpf==0x10) ){
        data[1] = hpf + rng;
        setStandby();
        writeRegs(data, 2);
        setActive();
    }
    if(rng==SCALE_RANGE_2G){
        return(4096);
    } else if(rng==SCALE_RANGE_4G){
        return(2048);
    } else if(rng==SCALE_RANGE_8G){
        return(1024);
    } else {
        return(1);
    }
}
void MMA8451Q_A::setPulse(uint8_t thx,uint8_t thy,uint8_t thz,uint8_t tml){
    uint8_t t[2];
    uint8_t val;

    setStandby();
    t[0]=REG_PULSE_CFG;
    t[1]=0x40; // Eventflag enabeled
    if(thx>0) t[1]+=0x01; // single tap x enabele
    if(thy>0) t[1]+=0x04; // single tap y enabele
    if(thz>0) t[1]+=0x10; // single tap z enabele
    writeRegs(t,2);
    t[0]=REG_PULSE_THSX;
    t[1]=thx; // single tap enabele
    writeRegs(t,2);
    t[0]=REG_PULSE_THSY;
    t[1]=thy; // single tap enabele
    writeRegs(t,2);
    t[0]=REG_PULSE_THSZ;
    t[1]=thz; // single tap enabele
    writeRegs(t,2);
    //PULSE_TMLT: タップ持続時間 ≒tml * 0.625ms for Pulse_LPF_EN=0
    t[0]=REG_PULSE_TMLT;
    t[1]=tml; // single tap enabele
    writeRegs(t,2);
    /*
    t[0]=REG_PULSE_LTCY;
    t[1]=0; // single tap enabele
    writeRegs(t,2);
    t[0]=REG_PULSE_WIND;
    t[1]=0; // single tap enabele
    writeRegs(t,2);
    */
    t[0]=REG_CTRL_REG_4;
    val = readReg1(t[0]);
    t[1]=val | 0x08; // INT_EN_PULSE enabled
    writeRegs(t,2);
    t[0]=REG_CTRL_REG_5;
    val = readReg1(t[0]);
    t[1]=val | 0x08;  // INT1/INT2 Configuration. Default value: 0.
                //0: Interrupt is routed to INT2 pin; 1: Interrupt is routed to INT1 pinsingle tap enabele
    writeRegs(t,2);

    setActive();
}

void MMA8451Q_A::setTrans(uint8_t ths,uint8_t count){
    uint8_t t[2];
    uint8_t val;
    setStandby();
    t[0]=REG_TRANSIENT_CFG;
    t[1]=0x1E; // TRANSIENT_Cnfig xyz and hpf no bypass
    writeRegs(t,2);
    t[0]=REG_TRANSIENT_THS;
    t[1]=ths; // set transient threshold 1-127 0.063g/bit
    writeRegs(t,2);
    t[0]=REG_TRANSIENT_COUNT;
    t[1]=count; // set DeBounce count 0-255 1.25ms/bit
    writeRegs(t,2);
    t[0]=REG_CTRL_REG_4;
    val = readReg1(t[0]);
    t[1]=val | 0x20; // INT_EN_TRANS enabled
    writeRegs(t,2);
    t[0]=REG_CTRL_REG_5;
    val = readReg1(t[0]);
    t[1]=val | 0x20;  // INT1/INT2 Configuration. Default value: 0.
                //0: Interrupt is routed to INT2 pin; 1: Interrupt is routed to INT1 pinsingle tap enabele
    writeRegs(t,2);

    setActive(); 
}
uint8_t MMA8451Q_A::readReg1(int addr){
    char t[1];
    t[0] = addr;
    m_i2c.write(m_addr,t,1,true);
    m_i2c.read(m_addr,t,1);
    return(t[0]);
}
void MMA8451Q_A::writeReg1(int addr,int val){
    char t[2];
    t[0]=addr;
    t[1]=val;
    m_i2c.write(m_addr,t,2);
}



void MMA8451Q_A::readRegs(int addr, uint8_t * data, int len) {
    char t[1];
    t[0] = addr;
    m_i2c.write(m_addr, t, 1, true);
    m_i2c.read(m_addr, (char *)data, len);
}

void MMA8451Q_A::writeRegs(uint8_t * data, int len) {
    m_i2c.write(m_addr, (char *)data, len);
}
