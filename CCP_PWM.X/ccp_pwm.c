/*
 * File:   ccp_pwm.c
 * Author: esteban
 *
 * Created on May 30, 2023, 11:32 AM
 */
#include <xc.h>

#define _XTAL_FREQ 1000000
#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
//#pragma config CCP2MX = OFF // RB3 as CCP2 (RC2)

void main(void) {
    TRISC2 = 0; // as output CCP1
    TRISC1 = 0; // as output CCP2
    TRISC0 = 0; // as output
    
    PR2 = 249;  // Period register
    
    CCPR2L = 125; // D = 50%
    CCPR1L = 50; // D = 20%
  
    T2CON = 0b00000000; // PS=1 and power off
    
    CCP1CON = 0b00001100; // PWM mode
    CCP2CON = 0b00001100; // PWM mode
    
    TMR2 = 0; // TMR2 start in cero
    TMR2ON = 1; 

    while(1){
        // If you need other PWM, Arduino has this way.
//        LATC0 = 1;
//        __delay_us(500);
//        LATC0 = 0;
//        __delay_us(500);
    }
    
}
