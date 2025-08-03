/*

*/
/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MMA8451Q_A_H
#define MMA8451Q_A_H

#define REG_STATUS          0x00
#define REG_OUT_X_MSB       0x01
#define REG_OUT_Y_MSB       0x03
#define REG_OUT_Z_MSB       0x05
#define REG_FIFO_SETUP      0X09
#define REG_TRIG_CONF       0x0A
#define REG_SYSMOD          0x0B
#define REG_INT_SOURCE      0x0C
#define REG_WHO_AM_I        0x0D
#define REG_XYZ_DATA_CFG    0x0E
#define REG_HP_FLT_CUTOFF   0x0F
#define REG_PL_STATUS       0x10
#define REG_PL_CONFIG       0x11
#define REG_PL_COUNT        0x12
#define REG_PL_BF_ZCOMP     0x13
#define REG_P_L_THS_REG     0x14
#define REG_FF_MT_CFG       0x15
#define REG_FF_MT_SRC       0x16
#define REG_FF_MT_THS       0x17
#define REG_FF_MT_COUNT     0x18
#define REG_TRANSIENT_CFG   0x1D
#define REG_TRANSIENT_SCR   0x1E
#define REG_TRANSIENT_THS   0x1F
#define REG_TRANSIENT_COUNT 0x20
#define REG_PULSE_CFG       0x21
#define REG_PULSE_SRC       0x22
#define REG_PULSE_THSX      0x23
#define REG_PULSE_THSY      0x24
#define REG_PULSE_THSZ      0x25
#define REG_PULSE_TMLT      0x26
#define REG_PULSE_LTCY      0x27
#define REG_PULSE_WIND      0x28
#define REG_ASLP_COUNT      0x29
#define REG_CTRL_REG_1      0x2A
#define REG_CTRL_REG_2      0x2B
#define REG_CTRL_REG_3      0x2C
#define REG_CTRL_REG_4      0x2D
#define REG_CTRL_REG_5      0x2E
#define REG_OFF_X           0x2F
#define REG_OFF_Y           0x30
#define REG_OFF_Z           0x31

#define SCALE_RANGE_2G      0x00
#define SCALE_RANGE_4G      0x01
#define SCALE_RANGE_8G      0x02
#define HPF_ON              0x10
#define HPF_OFF             0x00

#include "mbed.h"
#include <cstdint>

class MMA8451Q_A
{
public:
  /**
  * MMA8451Q constructor
  *
  * @param sda SDA pin
  * @param sdl SCL pin
  * @param addr addr of the I2C peripheral
  */
  MMA8451Q_A(PinName sda, PinName scl, int addr);

  /**
  * MMA8451Q destructor
  */
  ~MMA8451Q_A();

  uint8_t setActive();
  uint8_t setStandby();
//void setPulseIntMode();
  
  /**
   * Get the value of the WHO_AM_I register
   * 
   * @returns WHO_AM_I value( fixed 0x1A )
   */
  uint8_t getWhoAmI();

  uint8_t getTest();

  /**
   * SingleTap interrupt enbale setting
   * @param thx threshold X(1-127) 0.063g/bit. 0 is disable
   * @param thy threshold Y(1-127) 0.063g/bit. 0 is disable
   * @param thz threshold Z(1-127) 0.063g/bit. 0 is disable
   * @param tml tap continuous time 0.625ms/bit 
   */
  void setPulse(uint8_t thx,uint8_t thy,uint8_t thz,uint8_t tml);

  /**
   * Transient interrupt enable setting
   * @param ths threshold 1-128 0.063g/bit
   * @param count deBounce count 0-255 1.25ms/bit (不感時間)
   */
  void setTrans(uint8_t ths,uint8_t count);

  /**
   * Get X axis acceleration
   * @returns X axis acceleration
   */
  int16_t getAccX();

  /**
   * Get Y axis acceleration
   * @returns Y axis acceleration
   */
  int16_t getAccY();

  /**
   * Get Z axis acceleration
   * @returns Z axis acceleration
   */
  int16_t getAccZ();

  /**
   * Get XYZ axis acceleration
   * int16_t res[3]
   * @param res array where acceleration data will be stored
   */
  void getAccAllAxis(int16_t * res);
  
  /**
   * Set Full-scale range (2G/4G/8G)
   * @param rng range SCALE_RANGE_2G(default)/SCALE_RANGE_4G/SCALE_RANGE_8G
   * @param hpf HPF_ON/HPF_OFF(default)
   * @returns resolution ex.) set 2G : 4096.0
   */
  float setRange(uint8_t rng,uint8_t hpf);

  uint8_t readReg1(int addr);
  void writeReg1(int addr,int val);

private:
  I2C m_i2c;
  int m_addr;
  void readRegs(int addr, uint8_t * data, int len);
  void writeRegs(uint8_t * data, int len);
  int16_t getAccAxis(uint8_t addr);

};

#endif
