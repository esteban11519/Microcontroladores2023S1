#include <xc.h>
#define _XTAL_FREQ 8000000 // Fosc = 8MHz; define before of LibLCDXC8.h
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT = OFF
#pragma config WDTPS = 1024 // T = 1/(31kHz/(128*WDTPS)) (4,22 s), [2^0, 2^13] discrete


// Global variables
unsigned char Key=0;
unsigned char Check_alarm=1;


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


/* Principal functions */
void check_ultrasonic(unsigned char *, unsigned char, unsigned char , unsigned char);
void verify_alarm_status(unsigned char, unsigned char);
void select_options(unsigned char *, unsigned char);
void do_select_options(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *);


/* Main sub functions */
void init_configuration(void);
void welcome_operations(void); 

unsigned char ult_son_get_dis(void); // Please WDT
void send_RS232(unsigned char *); // [x]

void signals_status_alarm(unsigned char,unsigned char *);

unsigned char is_sym_val_opt(unsigned char *, unsigned char);


/* Interrupt functions */
unsigned char read_key(void);
unsigned char key2symbol(unsigned char);

/* low and high priorities */
void __interrupt(low_priority) ISR_low(void);
void __interrupt(high_priority) ISR_high(void);
			       
void main(void) {
  init_configuration();
  welcome_operations();

  // Local variables
  unsigned char key=0; // Save the Key 
  unsigned char is_secure_ultrasonic_distance=0;
  unsigned char ultrasonic_distance_permitted=7; // [cm]
  unsigned char dx_ultrasonic_distance_permitted=1; // [cm]
  unsigned char status_alarm = 0; // 1 on, 0 off
  unsigned char option_selected = 0;
  unsigned char option_selected_previous = 0;
  unsigned char buffer_password_inserted [9]; // Buffer to password inserted
  unsigned char current_password[9]="01234567";
  unsigned char operation_password_success=0;
  /* Buffer to LCD*/
  unsigned char buffer_LCD_row_1[17];
  unsigned char buffer_LCD_row_2[17];

  // Activate watchdog
  SWDTEN = 1;
  while(1){

    if(Check_alarm){
      
      // Update key
      key=Key;
      Key=0;

      check_ultrasonic(&is_secure_ultrasonic_distance, status_alarm, ultrasonic_distance_permitted, dx_ultrasonic_distance_permitted);
      CLRWDT();
      
      verify_alarm_status(is_secure_ultrasonic_distance,status_alarm);
      
      select_options(&option_selected, key);

      do_select_options(&option_selected, &option_selected_previous, buffer_password_inserted,current_password, &operation_password_success,&status_alarm);

      // update others variables
      if(option_selected_previous!=option_selected)
	option_selected_previous=option_selected;
      
    }
    
    // Test ultrasonic
    // BorraLCD();
    // EscribeLCD_n8(ult_son_get_dis(),3);
    //CLRWDT(); // Clear Watchdog Timer
    
    // Test keyboard
    /* BorraLCD(); */
    /* if(Key!=0) */
    /*   EscribeLCD_c(Key); */
    /* Key=0; */
    
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
  CCPR1L=1; // Ton=(PR2+1)*D (D: Duty cycle)
  CCP1CON=0b00001100; //<7:4>--00, <3:0> 11xx PWM
  TMR2ON=0;
  
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
  TMR0IP=1; // High priority 


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

  /* TMR0 */
  TMR0IE=1; 
  // Enable timer
  TMR0ON=1; 

  
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


void check_ultrasonic(unsigned char *is_secure_ultrasonic_distance, unsigned char status_alarm, unsigned char ultrasonic_distance_permitted, unsigned char dx_ultrasonic_distance_permitted){

  int relative_distance=ultrasonic_distance_permitted-ult_son_get_dis();

  if(relative_distance > dx_ultrasonic_distance_permitted || relative_distance < (int)(-dx_ultrasonic_distance_permitted))
    *is_secure_ultrasonic_distance=0;
  else
    *is_secure_ultrasonic_distance=1;
  
  return;
}

void verify_alarm_status(unsigned char is_secure_ultrasonic_distance, unsigned char status_alarm){
  if(is_secure_ultrasonic_distance==0 && status_alarm==1){
    // Activate buzzer
    TMR2ON=1;
    CCPR1L=62;
    
  }
  else{
    // deactivate buzzer
    TMR2ON=0;
    CCPR1L=1;
  }
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

void signals_status_alarm(unsigned char status_alarm, unsigned char *buffer_password_inserted){

  // Also can give information when put numbers.
  if(status_alarm==1){
    // Green
    RGB_R=1;
    RGB_G=0;
    RGB_B=1;
  }
  else if(status_alarm==0){
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

unsigned char is_sym_val_opt(unsigned char *arr, unsigned char sym){
  unsigned char counter=0;
  while(arr[counter]!='\0'){
    if(arr[counter]==sym) return 1;
    counter++;
  }
  return 0;
}

void select_options(unsigned char *option_selected, unsigned char key){
  /*
    A: Enter
    B: Insert password
    C: Change password
    D: Delete numbers
    *: Cancel
    #: Activate
   */
  unsigned char valid_sym_opt[]={'A','B','C','D','*','#','\0'};
  if(is_sym_val_opt(valid_sym_opt,key))
    *option_selected=key;
  else
    *option_selected='B';
      
  return;
}

void do_select_options(unsigned char *option_selected, unsigned char *option_selected_previous, unsigned char *buffer_password_inserted, unsigned char *current_password, unsigned char *operation_password_success, unsigned char *status_alarm){

  unsigned char counter=0;
  
  if(*option_selected_previous=='B' && *option_selected=='A'){
    // Insert password and enter
    while(current_password[counter]!='\0'){
      if(current_password[counter]!=buffer_password_inserted[counter]){
	*operation_password_success=0;
	buffer_password_inserted[0]='\0';
	*option_selected='\0';
	break;
      }
      *operation_password_success=1;
      
    }
    buffer_password_inserted[0]='\0';
    *option_selected='\0';
  }
  else if(*option_selected_previous=='C' && *option_selected=='A'){
   // Change password and enter
    while(current_password[counter]!='\0'){
      current_password[counter]=buffer_password_inserted[counter];
      counter++;
    }
    
    *operation_password_success=1;
    buffer_password_inserted[0]='\0';
    *option_selected='\0';
  }
  else if(*option_selected=='C'){
    // Change password
  }
  else if(*option_selected=='B'){
    // Insert password
  }
  else if(*option_selected=='D'){
    // Delete all
    buffer_password_inserted[0]='\0';
    
  }
  else if(*option_selected=='*'){
    //cancel delete all
    buffer_password_inserted[0]='\0';
    *option_selected='\0';
    *option_selected_previous='\0';
    *operation_password_success='\0';
  }
  else if(*option_selected=='#'){
    *status_alarm=1;
  }
 
  return;
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
    if(TMR0IF==1){
    TMR0=3036;
    TMR0IF=0;
    Check_alarm=1;
  }

  return;
}
