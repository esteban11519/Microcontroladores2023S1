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

unsigned char LeerTeclado(void);
void main(void){
  /* Variables */
  unsigned char Tecla=0;
  
  /* LCD configuration */
  TRISD=0;			/*PortD as output */
  LATD=0;			/* PortD inir in 0 logic */
  
  ConfiguraLCD(4);
  InicializaLCD();

  
  /* Keyboard configuration */
  TRISB=0b11110000;	     /*  RB7:RB4 as input and RB3:RB0 as output */
  LATB=0b00000000;	     /* RB3:RB0 in 0 logic */
  RBPU=0;			/*  PORTB Pull-up Enable bit, 0 is enable pullup resistence */

  /* General configuration */
  __delay_ms(1000);		/* LCD stabilize time (1000 ms) and pull-up resistences (100 ms)*/


  /* Welcome message */
  MensajeLCD_Var("Hola Mundo");
  DireccionaLCD(0xC0); 
  MensajeLCD_Var("Dios es bueno");

  __delay_ms(1000);
  BorraLCD();
  
  while(1){
    Tecla=LeerTeclado();
    if(Tecla!=0){
      EscribeLCD_n8(Tecla,2);
      __delay_ms(1000); 
      Tecla=0;
      BorraLCD();
    }
      
  }    
}


unsigned char LeerTeclado(void){

  LATB=0b11111110;
  __delay_ms(10); 		/* Delay to stabilize */
  if(RB4==0) return 1;
  if(RB5==0) return 2;
  if(RB6==0) return 3;
  if(RB7==0) return 4;
  LATB=0b11111101;
  __delay_ms(10);
  if(RB4==0) return 5;
  if(RB5==0) return 6;
  if(RB6==0) return 7;
  if(RB7==0) return 8;
  LATB=0b11111011;
  __delay_ms(10);
  if(RB4==0) return 9;
  if(RB5==0) return 10;
  if(RB6==0) return 11;
  if(RB7==0) return 12;
  LATB=0b11110111;
  __delay_ms(10);
  if(RB4==0) return 13;
  if(RB5==0) return 14;
  if(RB6==0) return 15;
  if(RB7==0) return 16;

  return 0;
}
