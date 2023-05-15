/*
  This code is adapted from Blanco Robin proffesor codes.
*/
#include<xc.h>
#define _XTAL_FREQ 1000000 // Fosc = 1MHz; define before of LibLCDXC8.h
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC 	/*Internal oscillator 1 MHz */
#pragma config WDT=OFF		/* Watchdog off  */
#pragma config PBADEN=OFF	/* RB0 to RB4 as digital i/o */
#pragma config LVP=OFF		/* RB5 as digital i/o */

// Global variables
unsigned char Tecla=0;

// Interruption Service Rutine
void __interrupt() ISR(void);

void main(void){
  
  /* LCD configuration */
  TRISD=0;			/*PortD as output */
  LATD=0;			/* PortD inir in 0 logic */
  
  ConfiguraLCD(4);
  InicializaLCD();

  
  /* Keyboard configuration */
  TRISB=0b11110000;	     /*  RB7:RB4 as input and RB3:RB0 as output */
  LATB=0b00000000;	     /* RB3:RB0 in 0 logic */
  RBPU=0;			/*  PORTB Pull-up Enable bit, 0 is enable pullup resistence */

  RBIF=0; 			// RB Port Change Interrupt Flag bit, 0:  None of the RB7:RB4 pins have changed state
  RBIE=1;			// RB Port Change Interrupt Enable bit, 1: Enables the RB port change interrupt
  GIE=1;  			// Global Interrupt Enable bit, 1: = Enables all unmasked interrupts
  
  /* General configuration */
  __delay_ms(1000);		/* LCD stabilize time (1000 ms) and pull-up resistences (100 ms)*/


  /* Welcome message */
  MensajeLCD_Var("Hola Mundo");
  DireccionaLCD(0xC0); 
  MensajeLCD_Var("Dios es bueno");

  __delay_ms(1000);
  BorraLCD();
  
  while(1){
    EscribeLCD_n8(Tecla,2);
    SLEEP();
    BorraLCD();
  }    
}

void aux_search_key(void){
  // Thanks to https://www.tutorialspoint.com/cprogramming/c_array_of_pointers.htm
  unsigned char *numbers_key[]={1,2,3,4,
				5,6,7,8,
				9,10,11,12,
				13,14,15,16};
  unsigned char *output_LATB_values[]={0b11111110,
				       0b11111101,
				       0b11111011,
				       0b11110111};
  for(unsigned char i=0; i<4; i++){
    LATB=output_LATB_values[i];
    __delay_ms(10); 		//stabilize LATB 
    
    if(RB4==0) {Tecla=numbers_key[i*4];break;} 
    if(RB5==0) {Tecla=numbers_key[i*4+1];break;}
    if(RB6==0) {Tecla=numbers_key[i*4+2];break;}
    if(RB7==0) {Tecla=numbers_key[i*4+3];break;}
  }

  return;

}

void __interrupt() ISR(void){
  unsigned char aux_PORTB=PORTB; 
  if(RBIF==1){
    // Falling edge discrimination
    if((aux_PORTB & 0b11110000)!=0b11110000){
      Tecla=0;
      aux_search_key();
      LATB=0b11110000;
    }
    __delay_ms(100);
    RBIF=0;
  }
  return;
}
