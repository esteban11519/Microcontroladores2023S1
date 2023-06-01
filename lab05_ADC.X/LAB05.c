/*
 * File:   LAB05.c
 * Author: Admin
 *
 * Created on 30 de mayo de 2023, 09:12 AM
 */

#include <xc.h>
//#include "LibLCDXC8.h"
#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma PBADEN=OFF
#pragma LVP=OFF

// DHT11 pines
#define DATA_DIR TRISC0
#define DATA_IN RC0
#define DATA_OUT LATC0

// RGB pines
#ifndef RGB_R
#define RGB_R RE0
#endif
#ifndef RGB_G
#define RGB_G RE2
#endif
#ifndef RGB_B
#define RGB_B RE1
#endif

// Interrutor pines
#ifndef INT_1
#define INT_1 RE0
#endif
#ifndef INT_2
#define INT_2 RE2
#endif


// Global variables
const unsigned long T_DHT11=2000; // Period of samples DHT11 [ms]

unsigned char Temp,Hum,Che;

unsigned char Buffer_RS232[16]; // Information ASCII to send by RS232
unsigned char Buffer_LCD[16]; // Information ASCII to send to LCD

// Functions
// To DHT11
void LeerHT11(void);
unsigned char LeerByte(void);
unsigned char LeerBit(void);
unsigned char Check(void);
void Transmitir (unsigned char);

void init_configuration(void);	//Init configuration
void measure_temperature_c(void); // Temperature in celsius
void check_potentiometer_voltaje(void);
void save_temperature_c_EEPROM(void);
void show_temperature_c_RGB(void);
void choose_scale_temperature(void);
void send_temperature_RS232(void);
void show_temperature_LCD(void);

// main
void main(void) {
  init_configuration();
  while(1){
    measure_temperature_c();
    check_potentiometer_voltaje();
    save_temperature_c_EEPROM();
    show_temperature_c_RGB();
    choose_scale_temperature();
    send_temperature_RS232();
    show_temperature_LCD();

  }
}


// Functions declaration
void init_configuration(void){
  OSCCON=0b01110000;
  __delay_ms(1);
  DATA_OUT=0;
  TXSTA=0b00100000;
  RCSTA=0b10000000;
  BAUDCON=0b00000000;
  SPBRG=12;
  return;
}


void measure_temperature_c(void){
  LeerHT11();
  return;
}

void check_potentiometer_voltaje(void){
  return;
}
void save_temperature_c_EEPROM(void){
  return;
}
void show_temperature_c_RGB(void){
  if(Temp<22){
    // Black
    RGB_R=1;
    RGB_G=1;
    RGB_B=1;
  }
  else if(Temp<25) {
    // Magenta
    RGB_R=0;
    RGB_G=1;
    RGB_B=0;
  }
  else if(Temp<28){
    // blue 
    RGB_R=1;
    RGB_G=1;
    RGB_B=0;
     
  }
  else if(Temp<31){
    // cyan
    RGB_R=1;
    RGB_G=0;
    RGB_B=0;
  }
  else if(Temp<34){
    // green
    RGB_R=1;
    RGB_G=0;
    RGB_B=1;
  }
  else if(Temp<37){
    // Yellow
    RGB_R=0;
    RGB_G=0;
    RGB_B=1;
  }
  else if(Temp<40){
    // red
    RGB_R=0;
    RGB_G=1;
    RGB_B=1;
  }
  else{
    // white
    RGB_R=0;
    RGB_G=0;
    RGB_B=0;
  }
    
  return;
}

void choose_scale_temperature(void){

  if(INT_1==0 && INT_2==0){
    // Celsius to celsius
  }
  else if(INT_1==0 && INT_2==1){
    // Celsius to Kelvin
  }
  else if(INT_1==1 && INT_2==0){
    // Celsius to Rankine
  }
  else if(INT_1==1 && INT_2==1){
    // Celsius to Fahrenheit
  }
  
  return;
}

void send_temperature_RS232(void){
  unsigned char counter=0;

  while(Buffer_RS232[counter]!='\0' && counter<255){
    Transmitir(Buffer_RS232[counter]);
    counter++;
  }
  return;
}

void show_temperature_LCD(void){
  return;
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
  return;
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
  return;
}


// void trasmitir_temp_c(void){
//   LeerHT11();
//   Transmitir('T');
//   Transmitir('e');
//   Transmitir('m');
//   Transmitir('p');
//   Transmitir(':');
//   Transmitir(' ');
//   Transmitir(Temp/10 + 48);
//   Transmitir(Temp%10 + 48);
//   Transmitir(' ');
//   Transmitir('C');
//   Transmitir('\n');
//   Transmitir('H');
//   Transmitir('u');
//   Transmitir('m');
//   Transmitir(':');
//   Transmitir(' ');
//   Transmitir(Hum/10 + 48);
//   Transmitir(Hum%10 + 48);
//   Transmitir(' ');
//   Transmitir('%');
//   Transmitir('\n');
//   __delay_ms(T_DHT11);
//   return;
// }


// void trasmitir_temp_K(void){
//   LeerHT11();
//   Transmitir('T');
//   Transmitir('e');
//   Transmitir('m');
//   Transmitir('p');
//   Transmitir(':');
//   Transmitir(' ');
//   Transmitir(Temp/10 + 48);
//   Transmitir(Temp%10 + 48);
//   Transmitir(' ');
//   Transmitir('K');
//   Transmitir('\n');
//   Transmitir('H');
//   Transmitir('u');
//   Transmitir('m');
//   Transmitir(':');
//   Transmitir(' ');
//   Transmitir(Hum/10 + 48);
//   Transmitir(Hum%10 + 48);
//   Transmitir(' ');
//   Transmitir('%');
//   Transmitir('\n');
//   __delay_ms(T_DHT11);
//   return;
// }
