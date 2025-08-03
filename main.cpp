/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "MMA8451Q_A.h"

// Blinking rate in milliseconds
//#define MMA8451_I2C_ADDRESS (0x1d<<1)
#define MMA8451_I2C_ADDRESS (0x1c<<1)
//Serial pc(USBTX,USBRX);
MMA8451Q_A acc(PTE25,PTE24,MMA8451_I2C_ADDRESS);
//MMA8451Q_A acc(PTE0,PTE1,MMA8451_I2C_ADDRESS);
InterruptIn acc_int1(PTD0);
float reso=4096;
uint8_t pulse_src;
uint8_t det_int=0;
void on_pulse(){
//    pulse_src = acc.readReg1(REG_PULSE_SRC);// interrupt flag clear
    det_int = 1;
}
int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
    led=0;
    acc_int1.mode(PullUp);
    acc_int1.fall(&on_pulse);   // REG_CTRL_REG_3のデフォルト(IPOL)がfallになっている
    
    int16_t xyz[3];
    reso = acc.setRange(SCALE_RANGE_2G, HPF_OFF);  // default=2G
    acc.setPulse(5, 5, 5, 5);
    printf("Start\n\r");
    
    uint8_t val;
    val = acc.getWhoAmI();
    printf("%0X\n\r",val);

    while (true) {
        acc.getAccAllAxis(xyz);
        printf("%5d,%5d,%5d\n\r",xyz[0],xyz[1],xyz[2]);
        if(det_int>0){
            pulse_src = acc.readReg1(REG_PULSE_SRC);// interrupt flag clear
            // EA AxZ AxY AxX DPE PolZ PolY PolX
            det_int=0;
            printf("int %0X\n\r",pulse_src);
        }
        //led = !led;
        thread_sleep_for(250);

    }
}
