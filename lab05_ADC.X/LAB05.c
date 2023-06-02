/*
 * File:   LAB05.c
 * Author: Admin
 *
 * Created on 30 de mayo de 2023, 09:12 AM
 */
#include <xc.h>
#include<stdio.h>
#define _XTAL_FREQ 8000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC

// https://www.electronicwings.com/pic/pic18f4550-watchdog-timer
#pragma config WDT = OFF
#pragma config WDTPS = 512 // T = 1/(31kHz/(128*WDTPS)) (2,11 s), [2^0, 2^13] discrete

// DHT11 pines
#define DATA_DIR TRISC1
#define DATA_IN RC1
#define DATA_OUT LATC1

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
#define INT_1 RA1
#endif
#ifndef INT_2
#define INT_2 RA2
#endif
 
// LED with frequency 1 [Hz]
#ifndef LEDT
#define LEDT RC2 	
#endif


// LED potentiometer
#ifndef LEDPOT
#define LEDPOT RB0 	
#endif

// Global variables

unsigned char Temp,Hum,Che;
unsigned char Buffer_RS232[12]; // Information ASCII to send by RS232
unsigned char Buffer_LCD[32]; // Information ASCII to send to LCD
unsigned int PreloadTMR0 = 3036;
unsigned char Enable_sample = 0;

// Functions
// To DHT11
void LeerHT11(void);
unsigned char LeerByte(void);
unsigned char LeerBit(void);
unsigned char Check(void);
void Transmitir (unsigned char);

void init_configuration(void);	// [x] Init configuration
void welcome_operations(void);  // [x]
void measure_temperature_c(void); // [x] Temperature in celsius
void check_potentiometer_voltaje(void); // [x]
void save_temperature_c_EEPROM(void); // []
void show_temperature_c_RGB(void); // [x]
void choose_scale_temperature_and_fill_buffers(void); // [x]
void send_temperature_RS232(void); // [x]
void show_temperature_LCD(void); // [x]

// Interruptions
void interrupt ISR(void);

// main
void main(void) {
  init_configuration();
  welcome_operations();

  // Activate watchdog
  //https://www.electronicwings.com/pic/pic18f4550-watchdog-timer
  SWDTEN = 1;
  
  while(1){
    
    if(Enable_sample){
    
      measure_temperature_c();
      // From: https://www.electronicwings.com/pic/pic18f4550-watchdog-timer
      CLRWDT(); // Clear Watchdog Timer

      check_potentiometer_voltaje();
      save_temperature_c_EEPROM();
      choose_scale_temperature_and_fill_buffers();
      send_temperature_RS232();
      show_temperature_c_RGB();
      show_temperature_LCD();
      Enable_sample = 0;
      
 
    }
    
  }
}


// Functions declaration
void init_configuration(void){
  // Clock configuration
  // <6-4> INTOSC = 8 MHz
  OSCCON=0b01110000;
  __delay_ms(1);

  // DHT11
  DATA_OUT=0;
  
  // RS232 
  TXSTA=0b00100000;
  RCSTA=0b10000000;
  BAUDCON=0b00000000;
  SPBRG=12;

  // AN12 to AN1 as digital i/o
  ADCON1 = 14 ;

  // RGB in RE0:RE2
  TRISE=0b11111000;
  LATE=1;
  
  // Interruptors Pines
  TRISA = 0xff;
  
  // LED with frequency 1 [Hz]
  TRISC2=0;
  LATC2=0;

  // LED potentiometer
  TRISB0=0;
  LATB0=0;

  // Potentiometer
  //ADCON0 — — CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
  ADCON0=0b00000001;
  // ADCON2: ADFM — ACQT2 ACQT1 ACQT0 ADCS2 ADCS1 ADCS0 
  ADCON2=0b11001000;
    
  // TM0 configuration
  // T0CON register
  // bit 7 - TMR0ON: 0-1, stop Timer()-enables Timer()
  // bit 6 - T08BIT: 0, 16 bits
  // bit 5 - T0CS: 0, Fbus
  // bit 4 - T0SE: 0, Increment on low-to-high transition on T0CKI pin ?
  // bit 3 - PSA: 0, Assigned prescaler
  // bit 2:0 - T0PS2:T0PS0: 001,1:4 Prescale value
  // bit 2:0 - T0PS2:T0PS0: 011,1:16 Prescale value
  // bit 2:0 - T0PS2:T0PS0: 011,1:16 Prescale value
  T0CON=0b00000100;
  TMR0=PreloadTMR0; 			//preload
  // INTCON
  // TMR0IF: 0, TMR0 register did not overflow
  // TMR0IE: 1, Enables the TMR0 overflow interrupt
  TMR0IF=0;			
  TMR0IE=1; 
  GIE=1;  			// Global Interrupt Enable bit, 1: = Enables all unmasked interrupts
  // Enable timer
  TMR0ON=1; 

  //LCD
  TRISD=0;			/*PortD as output */
  LATD=0;			/* PortD init in 0 logic */  
  ConfiguraLCD(4);
  InicializaLCD();
  __delay_ms(1000);		/* LCD stabilize time (1000 ms)*/

  return;
}

void welcome_operations(void){
  /* Welcome message */

  if(TO==0){
    TO=1;
    MensajeLCD_Var("Failed DHT11");
    SLEEP();
  }
  
  BorraLCD();
  MensajeLCD_Var("Bienvenido a Sensor");
  MensajeLCD_Var("Dios es bueno");
  __delay_ms(1000);
  BorraLCD();

   
  return;
}

void measure_temperature_c(void){
  LeerHT11();
  return;
}

void check_potentiometer_voltaje(void){
  ADON=1;
  GO=1;   //bsf ADCON0,1
  while(GO==1);
  BorraLCD();
  
  if(ADRES > 511)
    LEDPOT = 1;
  else
    LEDPOT = 0;
  
  ADON=0;
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


void choose_scale_temperature_and_fill_buffers(void){
  int temp_converted = 0;

  sprintf(Buffer_RS232, "T: %i C\n",Temp);
  sprintf(Buffer_LCD, "%s","Hola");
  
  if(INT_1==0 && INT_2==0){
    // Celsius to celsius
    sprintf(Buffer_LCD, "T: %i C\nH: %i %c",Temp,Hum,37);
  }
  else if(INT_1==0 && INT_2==1){
    // Celsius to Kelvin
    temp_converted = Temp + 273.15;
    sprintf(Buffer_LCD, "T: %i K\nH: %i %c", temp_converted,Hum,37);
  }
  else if(INT_1==1 && INT_2==0){
    // Celsius to Rankine
    temp_converted = Temp*1.8 + 491.67;
    sprintf(Buffer_LCD, "T: %i Ra\nH: %i %c", temp_converted,Hum,37);
  }
  else if(INT_1==1 && INT_2==1){
    // Celsius to Fahrenheit
    temp_converted = Temp*9.0/5.0 + 32.0;
    sprintf(Buffer_LCD, "T: %i F\nH: %i %c", temp_converted,Hum,37);
  }

  return;
}

void send_temperature_RS232(void){
  unsigned char counter=0;

  while(Buffer_RS232[counter]!='\0'){
    Transmitir(Buffer_RS232[counter]);
    counter++;
  }
  __delay_ms(200);
  return;
}

void show_temperature_LCD(void){
  unsigned char counter=0;
  BorraLCD();
  while (Buffer_LCD[counter]!='\0') {
    if(Buffer_LCD[counter]=='\n'){
      DireccionaLCD(0xC0);
      counter++;
    }
    EscribeLCD_c(Buffer_LCD[counter]);
    counter++;
  }
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

void interrupt ISR(void){
  if(TMR0IF==1){
    TMR0=PreloadTMR0;
    TMR0IF=0;
    LEDT=~LEDT;
    Enable_sample=1;
  }
  return;
}

