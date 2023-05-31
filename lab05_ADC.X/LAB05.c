/*
 * File:   LAB05.c
 * Author: Admin
 *
 * Created on 30 de mayo de 2023, 09:12 AM
 */


#include <xc.h>
#include "LibLCDXC8.h"
#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma PBADEN=OFF
#pragma LVP=OFF
#define DATA_DIR TRISC0
#define DATA_IN RC0
#define DATA_OUT LATC0

unsigned char Temp,Hum,Che;
unsigned char Dec,Uni;
void LeerHT11(void);
unsigned char LeerByte(void);
unsigned char LeerBit(void);
unsigned char Check(void);
void TrasmitirTempC(void);


void main(void) {
  OSCCON=0b01110000;
  __delay_ms(1);
  TRISD=0;
  LATD=0;
  DATA_OUT=0;
  TXSTA=0b00100000;
  RCSTA=0b10000000;
  BAUDCON=0b00000000;
  SPBRG=12;
  while(1){
      
      
    }
}


void LeerHT11(void){
  unsigned char i,contr=0;
  DATA_DIR=0;
  __delay_ms(18);
  DATA_DIR=1;
  while(DATA_IN==1);
  __delay_us(40);
  if(DATA_IN==0) contr++;
  __delay_us(80);
  if(DATA_IN==1) contr++;
  while(DATA_IN==1);
  Hum=LeerByte();
  LeerByte();
  Temp=LeerByte();
  LeerByte();
  Che=LeerByte();
}
unsigned char LeerByte(void){
  unsigned char res=0,i;
  for(i=8;i>0;i--){
    res=(res<<1) | LeerBit();  
  }
  return res;
}
unsigned char LeerBit(void){
  unsigned char res=0;
  while(DATA_IN==0);
  __delay_us(13);
  if(DATA_IN==1) res=0;
  __delay_us(22);
  if(DATA_IN==1){
    res=1;
    while(DATA_IN==1);
  }  
  return res;  
}
unsigned char Check(void){
  unsigned char res=0,aux;
  aux=Temp+Hum;
  if(aux==Che) res=1;
  return res;  
}
void Transmitir(unsigned char BufferT){
  while(TRMT==0);
  TXREG=BufferT;  
}
void TrasmitirTempC(void){
    __delay_ms(500);
    LATD0=1;
    __delay_ms(500);
    LATD0=0;
    LeerHT11();
    Transmitir('T');
    Transmitir('e');
    Transmitir('m');
    Transmitir('p');
    Transmitir(':');
    Transmitir(' ');
    Transmitir(Temp/10 + 48);
    Transmitir(Temp%10 + 48);
    Transmitir(' ');
    Transmitir('C');
    Transmitir('\n');
    Transmitir('H');
    Transmitir('u');
    Transmitir('m');
    Transmitir(':');
    Transmitir(' ');
    Transmitir(Hum/10 + 48);
    Transmitir(Hum%10 + 48);
    Transmitir(' ');
    Transmitir('%');
    Transmitir('\n');        
  }
void TrasmitirTempK(void){
    __delay_ms(500);
    LATD0=1;
    __delay_ms(500);
    LATD0=0;
    LeerHT11();
    Transmitir('T');
    Transmitir('e');
    Transmitir('m');
    Transmitir('p');
    Transmitir(':');
    Transmitir(' ');
    Transmitir(Temp/10 + 48);
    Transmitir(Temp%10 + 48);
    Transmitir(' ');
    Transmitir('K');
    Transmitir('\n');
    Transmitir('H');
    Transmitir('u');
    Transmitir('m');
    Transmitir(':');
    Transmitir(' ');
    Transmitir(Hum/10 + 48);
    Transmitir(Hum%10 + 48);
    Transmitir(' ');
    Transmitir('%');
    Transmitir('\n');        
  }
