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
#pragma config WDT = OFF
#pragma config WDTPS = 512 // T = 1/(31kHz/(128*WDTPS)) (2,11 s), [2^0, 2^13] discrete

// DHT11 pines
#define Data_Dir TRISC1
#define Data_In RC1
#define Data_Out LATC1

// RGB pines
#ifndef RGB_R
#define RGB_R LATE0
#endif
#ifndef RGB_G
#define RGB_G LATE2
#endif
#ifndef RGB_B
#define RGB_B LATE1
#endif

// Interrutor pines
#ifndef INT_1
#define INT_1 RA2
#endif
#ifndef INT_2
#define INT_2 RA3
#endif
 
// LED with frequency 1 [Hz]
#ifndef LEDT
#define LEDT LATC2 	
#endif


// LED potentiometer
#ifndef LEDPOT
#define LEDPOT LATB0 	
#endif

// Global variables

unsigned char Temp,Hum,Che;
unsigned char Enable_sample = 0;
unsigned char Scale_Temperature = 0;

// Functions
// To DHT11
void DHT11_Start(void);
void DHT11_CheckResponse(void);
char DHT11_ReadData(void);

//RS232
void Transmitir (unsigned char);

//EEPROM
void EEPROM_Write (unsigned char, unsigned char);
unsigned char EEPROM_Read (unsigned char);

// Main functions
void init_configuration(void);	// [x] Init configuration
void welcome_operations(void);  // [x]
void measure_temperature_c_humidity_per(void); // [x] Temperature in celsius and Humidity in percent
void send_temperature_RS232(void); // [x]
void save_temperature_c_EEPROM(void); // []
void choose_scale_temperature_and_fill_buffers(unsigned char *, unsigned char *); // [x]
void show_temperature_c_RGB(void); // [x]
void show_temperature_LCD(unsigned char *, unsigned char *, unsigned char, unsigned char, unsigned char); // [x]
void check_potentiometer_voltaje(void); // [x]



// Interruptions
void interrupt ISR(void);

// main
void main(void) {
  init_configuration();
  welcome_operations();
  
  // Activate watchdog
  SWDTEN = 1;

  // Variables
  // Information ASCII to send to LCD
  unsigned char buffer_LCD_row_1[17];
  unsigned char buffer_LCD_row_2[17];
  
  
  unsigned char Temp_n1=255; // Previous Temp
  unsigned char Hum_n1=255; // Previous Hum
  unsigned char Scale_Temperature_n1=255; // Previous Scale Temperature 
  
  while(1){
    
    if(Enable_sample){

      // Temp++;
      // if(Temp > 50) Temp = 18;
      // Hum+=10;
      measure_temperature_c_humidity_per();
      CLRWDT(); // Clear Watchdog Timer      
      send_temperature_RS232();
      save_temperature_c_EEPROM();
      choose_scale_temperature_and_fill_buffers(buffer_LCD_row_1,buffer_LCD_row_2);
      show_temperature_c_RGB();
      show_temperature_LCD(buffer_LCD_row_1, buffer_LCD_row_2, Temp_n1, Hum_n1, Scale_Temperature_n1);
      check_potentiometer_voltaje();
      Enable_sample = 0;

      // Update Temp_n1 and Hum_n1
      Temp_n1=Temp;
      Hum_n1=Hum;
      Scale_Temperature_n1=Scale_Temperature;

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
  
  // RS232 
  TXSTA=0b00100000;
  RCSTA=0b10000000;
  BAUDCON=0b00000000;
  SPBRG=12;

  // AN12 to AN2 as digital i/o
  ADCON1=13;

  // RGB in RE0:RE2
  TRISE=0b11111000;
  LATE=1;
  
  // Interruptors Pines
  
  // LED with frequency 1 [Hz]
  TRISC2=0;
  LATC2=0;

  // LED potentiometer
  TRISB0=0;
  LATB0=0;

  // Potentiometer
  //ADCON0 — — CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
  ADCON0=0b00000100;
  // ADCON2: ADFM — ACQT2 ACQT1 ACQT0 ADCS2 ADCS1 ADCS0 
  ADCON2=0b11001001;
  
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
  TMR0=3036; 			//preload
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

  // When Reset is give WatchDog Timer
  if(TO==0){
    TO=1;
    MensajeLCD_Var("Failed DHT11");
    SLEEP();
  }
  
  BorraLCD();
  MensajeLCD_Var("Sensors");
  MensajeLCD_Var("\nGod is good");
  __delay_ms(1000);

  // Show last temperature storaged in EEPROM
  BorraLCD();
  MensajeLCD_Var("Last Tempetature");
  MensajeLCD_Var("\nT: ");
  EscribeLCD_n8(EEPROM_Read(0x0),2);
  MensajeLCD_Var(" C");
  __delay_ms(1000);
 
  return;
}

void measure_temperature_c_humidity_per(void){
  DHT11_Start();  			/* send start pulse to DHT11 module */
  DHT11_CheckResponse();  	/* wait for response from DHT11 module */

  /* read 40-bit data from DHT11 module */
  Hum = DHT11_ReadData();  /* read Relative Humidity's integral value */
  DHT11_ReadData();   /* read Relative Humidity's decimal value */
  Temp = DHT11_ReadData();   /* read Temperature's integral value */
  DHT11_ReadData();    /* read Relative Temperature's decimal value */
  DHT11_ReadData();     /* read 8-bit checksum value */
    	
  return;
}


unsigned char DHT11_ReadData()
{
  unsigned char i,data = 0;  
    for(i=0;i<8;i++)
    {
        while(!(Data_In & 1));  /* wait till 0 pulse, this is start of data pulse */
        __delay_us(30);         
        if(Data_In & 1)  /* check whether data is 1 or 0 */    
          data = ((data<<1) | 1); 
        else
          data = (data<<1);  
        while(Data_In & 1);
    }
  return data;
}

void DHT11_Start()
{    
    Data_Dir = 0;  /* set as output port */
    Data_Out = 0;  /* send low pulse of min. 18 ms width */
    __delay_ms(18);
    Data_Out = 1;  /* pull data bus high */
    __delay_us(20);
    Data_Dir = 1;  /* set as input port */
    Data_Out = 0;  /* Data Out in cero */
    return;
}

void DHT11_CheckResponse()
{
    while(Data_In & 1);  /* wait till bus is High */     
    while(!(Data_In & 1));  /* wait till bus is Low */
    while(Data_In & 1);  /* wait till bus is High */
    return;
}

void check_potentiometer_voltaje(void){
  ADON=1;
  GO=1;
  while(GO==1);
  if(ADRES > 511)
    LEDPOT = 1;
  else
    LEDPOT = 0;
  ADON=0;
  return;
}

void save_temperature_c_EEPROM(void){
  EEPROM_Write(0x0, Temp);
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


void choose_scale_temperature_and_fill_buffers(unsigned char *buffer_LCD_row_1, unsigned char *buffer_LCD_row_2){
  int temp_converted = 0;
  //Prevent switch bounce
  __delay_ms(10);
  if(INT_1==0 && INT_2==0){
    // Celsius to celsius
    sprintf(buffer_LCD_row_1, "T: %i C",Temp);
    sprintf(buffer_LCD_row_2, "H: %i %c",Hum,37);

    Scale_Temperature=0;
  }
  else if(INT_1==0 && INT_2==1){
    // Celsius to Kelvin
    temp_converted = Temp + 273.15;

    sprintf(buffer_LCD_row_1, "T: %i K",temp_converted);
    sprintf(buffer_LCD_row_2, "H: %i %c",Hum,37);

    Scale_Temperature=1;
  }
  else if(INT_1==1 && INT_2==0){
    // Celsius to Rankine
    temp_converted = Temp*1.8 + 491.67;

    sprintf(buffer_LCD_row_1, "T: %i Ra",temp_converted);
    sprintf(buffer_LCD_row_2, "H: %i %c",Hum,37);

    Scale_Temperature=2;
    
  }
  else if(INT_1==1 && INT_2==1){
    // Celsius to Fahrenheit
    temp_converted = Temp*9.0/5.0 + 32.0;

    sprintf(buffer_LCD_row_1, "T: %i F",temp_converted);
    sprintf(buffer_LCD_row_2, "H: %i %c",Hum,37);

    Scale_Temperature=3;
  }

  return;
}

void send_temperature_RS232(void){
  unsigned char counter=0;
  unsigned char buffer_RS232[12];
  sprintf(buffer_RS232, "T: %i C\n",Temp);

  while(buffer_RS232[counter]!='\0'){
    Transmitir(buffer_RS232[counter]);
    counter++;
  }
  __delay_ms(200);
  return;
}

void show_temperature_LCD(unsigned char *buffer_LCD_row_1, unsigned char *buffer_LCD_row_2, unsigned char Temp_n1, unsigned char Hum_n1, unsigned char Scale_Temperature_n1){
  unsigned char counter=0;

  if(Temp_n1!=Temp || Scale_Temperature!=Scale_Temperature_n1){
    DireccionaLCD(0x80);
    // Write first row
    while (buffer_LCD_row_1[counter]!='\0') {
      EscribeLCD_c(buffer_LCD_row_1[counter]);
      counter++;
    }
    // clear first row
    while (counter<16) {
      EscribeLCD_c(' ');
      counter++;
    }
  }
  
  counter=0;

  if(Hum_n1!=Hum){
    DireccionaLCD(0xC0);

    // Write second row
    while (buffer_LCD_row_2[counter]!='\0') {
      EscribeLCD_c(buffer_LCD_row_2[counter]);
      counter++;
    }
    
    // clear second row
    while (counter<16) {
      EscribeLCD_c(' ');
      counter++;
    } 
  }

  return;
}



void Transmitir(unsigned char BufferT){
  while(TRMT==0);
  TXREG=BufferT;
  return;
}

void interrupt ISR(void){
  if(TMR0IF==1){
    TMR0=3036;
    TMR0IF=0;
    LEDT=~LEDT;
    Enable_sample=1;
  }
  return;
}

void EEPROM_Write (unsigned char address, unsigned char data){
  EEADR=address;
  EEDATA=data;
 
  WREN=1;

  EECON2=0x55;
  EECON2=0xAA;

  WR=1;
  while(WR==1);

  WREN=0;
  return;

}

unsigned char EEPROM_Read (unsigned char address){
  EEADR=address;
  RD=1;
  return EEDATA;
}

/*

  Bibliography
  WDT configuration : // https://www.electronicwings.com/pic/pic18f4550-watchdog-timer
  EEPROM : https://www.electronicwings.com/pic/pic18f4550-on-chip-eeprom
  DHT11 : https://www.electronicwings.com/pic/dht11-sensor-interfacing-with-pic18f4550
  Examples: https://github.com/lcgamboa/picsimlab_examples/tree/master/docs/board_Breadboard/PIC18F4550/test_b0/src/teste_b0.X
 */
