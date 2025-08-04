/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "MMA8451Q_A.h"
#include "USBSerial.h"

#define READING_TIMES 0.2
#define DATA_MAX 800

//#define MMA8451_I2C_ADDRESS (0x1d<<1)
#define MMA8451_I2C_ADDRESS (0x1c<<1)

USBSerial usbserial;
MMA8451Q_A acc(PTE25,PTE24,MMA8451_I2C_ADDRESS);
//MMA8451Q_A acc(PTE0,PTE1,MMA8451_I2C_ADDRESS);
//InterruptIn acc_int1(PTD0);
InterruptIn sw(PTA4);
Timeout stop_reading;

float reso=4096;
uint8_t pulse_src;
int det_int=0;
int det_tm=0; 
int16_t data[DATA_MAX][3];

void on_pulse(){
//    pulse_src = acc.readReg1(REG_PULSE_SRC);// interrupt flag clear
    det_int = 1;
}
void on_sw(){
    det_int = 1;
}
void readingTimeOut(){
    det_tm = 1;
}
int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
    led=0;
    //acc_int1.mode(PullUp);
    //acc_int1.fall(&on_pulse);   // REG_CTRL_REG_3のデフォルト(IPOL)がfallになっている
    sw.mode(PullUp);
    sw.fall( &on_sw );

    int16_t xyz[3];

    reso = acc.setRange(SCALE_RANGE_2G, HPF_OFF);  // default=2G
    //acc.setPulse(5, 5, 5, 5);
    printf("Start\n\r");
    
    uint8_t val;
    val = acc.getWhoAmI();
    printf("%0X\n\r",val);
    char buf[32];
    int buflen;

    while (true) {
        if(det_int>0){
            stop_reading.attach( callback(readingTimeOut),200ms);
            //pulse_src = acc.readReg1(REG_PULSE_SRC);// interrupt flag clear
            // EA AxZ AxY AxX DPE PolZ PolY PolX
            det_int=0;
            int kk=0;
            while(1){
                acc.getAccAllAxis(xyz);
                data[kk][0]=xyz[0];
                data[kk][1]=xyz[1];
                data[kk][2]=xyz[2];
                kk++;
                thread_sleep_for(1);
                //buflen = sprintf(buf,"%3d:%4d,%4d,%4d\n\r",kk,xyz[0],xyz[1],xyz[2]);
                //usbserial.printf("%3d:%5d,%5d,%5d\n\r",kk++,xyz[0],xyz[1],xyz[2]);
                //usbserial.write(buf,buflen);
                if(det_tm>0){
                    stop_reading.detach();
                    det_tm=0;
                    break;
                }
            }

            for(int i=0;i<kk;i++){
                buflen = sprintf(buf,"%3d:%4d,%4d,%4d\n\r",i+1,data[i][0],data[i][1],data[i][2]);
                usbserial.write(buf,buflen);
            }
        //    printf("int %0X\n\r",pulse_src);
        }
        //led = !led;
        //thread_sleep_for(250);

    }
}
