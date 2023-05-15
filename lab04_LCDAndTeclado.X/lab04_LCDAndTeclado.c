#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
void main(void){
  TRISD=0;
  LATD=0;
  __delay_ms(1000);
  ConfiguraLCD(4);
  InicializaLCD();
  MensajeLCD_Var("Hola Mundo");
  DireccionaLCD(0xC0); 
  MensajeLCD_Var("Dios es bueno");

  while(1){
  }    
}
