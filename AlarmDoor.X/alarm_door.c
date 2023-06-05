#include <xc.h>
#define _XTAL_FREQ 8000000 // Fosc = 8MHz; define before of LibLCDXC8.h
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT = OFF
#pragma config WDTPS = 1024 // T = 1/(31kHz/(128*WDTPS)) (4,22 s), [2^0, 2^13] discrete


// Ultrasonic sensor
#define TRIGGER RC0
#define ECHO RC1


void init_configuration(void);

void welcome_operations(void); 
unsigned char ult_son_get_dis(void); // Pleae WDT
void send_RS232(unsigned char *); // [x]

void main(void) {
  init_configuration();
  welcome_operations();
  // Activate watchdog
  SWDTEN = 1;

  while(1){
    BorraLCD();
    EscribeLCD_n8(ult_son_get_dis(),3);
    
    CLRWDT(); // Clear Watchdog Timer
    __delay_ms(1000);
  }
  
  
}

void init_configuration(void){
  // Clock configuration
  // <6-4> INTOSC = 8 MHz
  OSCCON=0b01110000;
  __delay_ms(1);

  // Ultrasonic Sensor
  TRISC0=0; // As output
  LATC0=0; // Init in cero
  //T1CON: TIMER1 CONTROL REGISTER
  //RD16 T1RUN T1CKPS1 T1CKPS0 T1OSCEN T1SYNC TMR1CSTMR1ON
  //<7>16 bits, <6>another source,<5-4> -preescalaler 2, <3> shut off, <2> Synchronize external clock, <1> Internal clock (Fbus), <0> Stop timer1 
  T1CON=0b10010000;

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


void send_RS232(unsigned char *data){
  unsigned char counter=0;
  while(data[counter]!='\0'){
    while(TRMT==0); // Wait register is empty
    TXREG=data[counter];
  }
  return;
}
