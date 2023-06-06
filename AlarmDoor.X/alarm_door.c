#include <xc.h>
#define _XTAL_FREQ 8000000 // Fosc = 8MHz; define before of LibLCDXC8.h
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT = OFF
#pragma config WDTPS = 1024 // T = 1/(31kHz/(128*WDTPS)) (4,22 s), [2^0, 2^13] discrete


// Global variables
unsigned char Key=0;
unsigned char Status_alarm=1; // 1 activate

// Ultrasonic sensor
#define TRIGGER LATC0
#define ECHO RC1

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



/* Main functions */
void init_configuration(void);
void welcome_operations(void); 

unsigned char ult_son_get_dis(void); // Please WDT
void send_RS232(unsigned char *); // [x]

void signals_status_alarm(void);

/* Interrupt functions */
unsigned char read_key(void);
unsigned char key2symbol(unsigned char);

/* low and high priorities */
void __interrupt(low_priority) ISR_low(void);
void __interrupt(high_priority) ISR_high(void);
			       
void main(void) {
  init_configuration();
  welcome_operations();
  // Activate watchdog
  SWDTEN = 1;

  while(1){
    // Test ultrasonic
    // BorraLCD();
    // EscribeLCD_n8(ult_son_get_dis(),3);
    CLRWDT(); // Clear Watchdog Timer
    
    // Test keyboard
    /* BorraLCD(); */
    /* if(Key!=0) */
    /*   EscribeLCD_c(Key); */
    /* Key=0; */
    
    __delay_ms(1000);
  }
  
  
}

void init_configuration(void){
  // Clock configuration
  // <6-4> INTOSC = 8 MHz
  OSCCON=0b01110000;
  __delay_ms(1);

  // RB0:RB4 and RE0:RE2 as i/o digital
  ADCON1=10;

  // RGB in RE0:RE2
  TRISE=0b11111000;
  LATE=1;

  // Keyboard configuration 
  TRISB=0b11110000; // RB7:RB4 as input and RB3:RB0 as output
  LATB=0b00000000; /*RB3:RB0 in 0 logic */
  RBPU=0;	   // PORTB Pull-up Enable bit, 0 is enable pullup resistence */

  // Ultrasonic Sensor
  TRISC0=0; // As output (TGU=)
  TRIGGER=0; // Init in cero
  //T1CON: TIMER1 CONTROL REGISTER
  //RD16 T1RUN T1CKPS1 T1CKPS0 T1OSCEN T1SYNC TMR1CSTMR1ON
  //<7>16 bits, <6>another source,<5-4> -preescalaler 2, <3> shut off, <2> Synchronize external clock, <1> Internal clock (Fbus), <0> Stop timer1 
  T1CON=0b10010000;

  // PWM
  TRISC2=0; // Use CCP1
  TMR2=0;   /* Timer2 is used with PWM */
  // T2CON: TIMER2 CONTROL REGISTER
  // - T2OUTPS3 T2OUTPS2 T2OUTPS1 T2OUTPS0 TMR2ON T2CKPS1 T2CKPS0
  // <7-3> - xxxx, off, 1x=16
  T2CON=0b00000010; // f=1Khz from fosc=8MHz equation: PR2 = T_PWM*F_bus/PS - 1
  PR2=124; // Period register
  CCPR1L=25; // Ton=(PR2+1)*D (D: Duty cycle)
  CCP1CON=0b00001100; //<7:4>--00, <3:0> 11xx PWM
  TMR2ON=1;
  
  // RS232
  // TXSTA : TRANSMIT STATUS AND CONTROL REGISTER
  // CSRC  TX9 TXEN(1) SYNC SENDB BRGH TRMT TX9D
  // <7> slave, <6> 8 bits data, <5> transmiter enable, <4> Asynchronous, <3> Sync Break transmission completed, <2> Low speed, <1> register is full, <0> only when send 9 bits.
  TXSTA=0b00100000;
  // RCSTA: RECEIVE STATUS AND CONTROL REGISTER
  // SPEN  RX9 SREN CREN ADDEN FERR OERR RX9D
  // <7> On, <6> 8 bits, <5> x, <4-3> not neccesary, <2-0> read register.
  RCSTA=0b10000000;
  // BAUDCON: BAUD RATE CONTROL REGISTER
  //ABDOVF RCIDL RXDTP TXCKP BRG16 — WUE ABDEN
  // <7> Not overflow, <6> Receptor in wait, <5> Received  Not inverted, <4> TX not inverted  <3> 8 bit to velocity, <2> -, <1> 0 disable low power, <0> Disable autotection velocity.  
  BAUDCON=0b00000000;
  // Baud Rate Generator, Is choose according to Velocity and Fosc. (Tables are uselfull)
  SPBRG=12;

  //LCD
  TRISD=0;			/*PortD as output */
  LATD=0;			/* PortD init in 0 logic */  
  ConfiguraLCD(4);
  InicializaLCD();
  __delay_ms(1000);		/* LCD stabilize time (1000 ms)*/

  /* Interruptions */
  /*Keyboard */
  RBIF=0; 			// RB Port Change Interrupt Flag bit, 0:  None of the RB7:RB4 pins have changed state
  RBIP=0;			// Low priority
  RBIE=1;			// RB Port Change Interrupt Enable bit, 1: Enables the RB port change interrupt
  
  IPEN=1;			// Enable priority levels on interrupts
  PEIE=1;			// Enable interruption bit
  GIE=1;  			// Global Interrupt Enable bit, 1: = Enables all unmasked interrupts

  
  return;
}

void welcome_operations(void){

// When Reset is give WatchDog Timer
  if(TO==0){
    TO=1;
    MensajeLCD_Var("Failed sensors");
    //SLEEP();
  }
  
  BorraLCD();
  MensajeLCD_Var("Alarm reset");
  MensajeLCD_Var("\nGod is good");
  __delay_ms(1000);

  return;

}
unsigned char ult_son_get_dis(void){
  //<7-6> - -, <5-4> Don't use, <3-0> Capture mode: every falling edge  
  CCP2CON = 0b00000100;
  TMR1=0;
  CCP2IF=0; // Flag interruption

  // Send pulse
  TRIGGER=1;
  __delay_us(10);
  TRIGGER=0;

  // Wait to receive echo signal
  while(ECHO==0);

  // Activate TMR1
  TMR1ON=1;

  // Measure pulse width
  while(CCP2IF==0);

  // Turn off TMR1
  TMR1ON=0;

  // CCPR has period of timer1 or timer3
  // 58 is factor when time is in us and velocity in cm/s
  return CCPR2/58;
  
}

void signals_status_alarm(void){

  // Also can give information when put numbers.
  if(Status_alarm==1){
    // Green
    RGB_R=1;
    RGB_G=0;
    RGB_B=1;
  }
  else if(Status_alarm==0){
    // RED
    RGB_R=0;
    RGB_G=1;
    RGB_B=1;
  }
  return;
}

void send_RS232(unsigned char *data){
  unsigned char counter=0;
  while(data[counter]!='\0'){
    while(TRMT==0); // Wait register is empty
    TXREG=data[counter];
  }
  return;
}

unsigned char key2symbol(unsigned char key){
  switch(key){
  case 1: return '1';
    break;
  case 2: return '2';
    break;
  case 3: return '3';
    break;
  case 4: return 'A';
    break;
  case 5: return '4';
    break;
  case 6: return '5';
    break;
  case 7: return '6';
    break;
  case 8: return 'B';
    break;
  case 9: return '7';
    break;
  case 10: return '8';
    break;
  case 11: return '9';
    break;
  case 12: return 'C';
    break;
  case 13: return '*';
    break;
  case 14: return '0';
    break;
  case 15: return '#';
    break;
  case 16: return 'D';
    break;
  default: return 0;
    break;
  }
  return 0;
}


unsigned char read_key(void){
  unsigned char key=0;

  unsigned char numbers_key[]={1,2,3,4,
      5,6,7,8,
      9,10,11,12,
      13,14,15,16};
  unsigned char output_LATB_values[]={0b11111110,
				      0b11111101,
				      0b11111011,
				      0b11110111};
  for(unsigned char i=0; i<4; i++){
    LATB=output_LATB_values[i];
    __delay_ms(10); 		/*stabilize LATB*/ 
    if(RB4==0) {key=numbers_key[i*4];break;} 
    if(RB5==0) {key=numbers_key[i*4+1];break;}
    if(RB6==0) {key=numbers_key[i*4+2];break;}
    if(RB7==0) {key=numbers_key[i*4+3];break;}
  }
  
  LATB=0b11110000;
  
  return key;
}

void __interrupt(low_priority) ISR_low(void){
  /*Polling */
  /* Keyboard interrupt */
  if(RBIF==1){
    Key=0;
    unsigned char aux_PORTB=PORTB;
    // Falling edge discrimination
    if((aux_PORTB & 0b11110000)!=0b11110000){
      Key=key2symbol(read_key());
    }
    __delay_ms(100);
    RBIF=0;
  }
  return;
}

void __interrupt(high_priority) ISR_high(void){
  return;
}
