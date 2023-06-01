/*
 * File:   ccp_compare.c
 * Author: esteban
 *
 * Created on May 30, 2023, 4:31 PM
 */
#include <xc.h>
#define _XTAL_FREQ 1000000
#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF

unsigned int a = 125; //0.5 ms

void interrupt ISR(void);

void main(void) {
    TRISC2 = 0; //RC2 as output
    
    TMR1 = 60536; // 20 ms
    T1CON = 0b10000000; // <7>: 16 bits, <6,5> prescaler 1 y <0>: turn off
    
    CCPR1 = 60536 + 125; // 20 + 0.5 [ms]
    
    // CCP1CON <3:0>Compare mode, initialize CCP1 pin high, clear output on compare match (set CCP1IF)
    CCP1CON = 0b00001001;
    
    // Interruption
    TMR1IF = 0; // Do not passed
    TMR1IE = 1; // Enable interruption
    PEIE = 1; // Enable periferical interruption
    GIE = 1; // 
    
    // turn on timer 1
    TMR1ON = 1;
    
    while(1){
        a=125; // 0.5 ms -> 0 grades
        __delay_ms(2000);
        
        a=375; // 1.0 ms -> 90 grades
        __delay_ms(2000);
        
        a=625; // 1.5 ms -> 180 grades
        __delay_ms(2000);
    } 
    
}

void interrupt ISR(void){
    TMR1IF = 0; 
    TMR1 = 60536;
    CCPR1 = 60536 + a;
    CCP1CON = 0b00001001; // again as compare mode
}