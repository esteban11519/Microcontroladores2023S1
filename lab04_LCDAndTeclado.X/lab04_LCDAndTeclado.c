/*
  This code is adapted from Blanco Robin proffesor codes.
*/
#include<xc.h>
#include<stdio.h>
//#include<math.h>
#define _XTAL_FREQ 1000000 // Fosc = 1MHz; define before of LibLCDXC8.h
#include "LibLCDXC8.h"

#ifndef LEDFRE
#define LEDFRE RC2 	
#endif

#ifndef RGB_R
#define RGB_R RE0
#endif

#ifndef RGB_G
#define RGB_G RE2
#endif

#ifndef RGB_B
#define RGB_B RE1
#endif


#pragma config FOSC=INTOSC_EC 	/*Internal oscillator 1 MHz */
#pragma config WDT=OFF		/* Watchdog off  */
#pragma config LVP=OFF		/* RB5 as digital i/o */

// Global variables
unsigned char Key;
unsigned int Preload;
unsigned char Sleep_counter_mode;
unsigned char Mode;


// Functions
void init_configuration(void);
void welcome_operations(void);
void show_reset_source(void);
void key2symbol(unsigned char *, unsigned char *);
unsigned char is_sym_val_dig(unsigned char *, unsigned char *);
unsigned char is_sym_val_ope(unsigned char *, unsigned char *);
unsigned char is_sym_val_res(unsigned char *, unsigned char *);
void print_array_char(char *);
void print_bad_dig(void);
void print_bad_ope(void);
void print_bad_res(void);
void calculate_and_show_result(unsigned char *, unsigned char *, unsigned char *, char *);
void commute_color(unsigned char *);
void clear_symbols(unsigned char *, unsigned char *,unsigned char *, unsigned char *);

// Bonus functions
unsigned long long int my_pow(unsigned char, unsigned char);
unsigned long long int my_factorial(unsigned char );

// Interruption Service Rutine
void __interrupt(low_priority) ISR_low(void);
void __interrupt(high_priority) ISR_high(void);

  
void main(void){
  // Init global variables
  Key=0;
  Preload=3036;
  Sleep_counter_mode=0;
  Mode=0;
  // init configuration
  init_configuration();
  
  // Variables
  // Thanks to : https://www.tutorialesprogramacionya.com/cya/detalleconcepto.php?punto=13&codigo=13&inicio=0
  // https://www.tutorialesprogramacionya.com/cya/detalleconcepto.php?punto=13&codigo=13&inicio=0
  unsigned char auxKey = 0 ;
  unsigned char symbol = 'F';  
  unsigned char dig_1 = 'F';
  unsigned char sym_ope = 'F';
  unsigned char dig_2 = 'F';
  unsigned char sym_res = 'F';

  char valid_sym_dig[]={1,2,3,4,5,6,7,8,9,0,'F'};
  char valid_sym_ope[]="+-X/^!"; 
  char valid_sym_res = '=';		      

  unsigned char color = 0;

  
  // 'Aux' variables
  unsigned char rewrite=0;
  char aux_view_lcd[12];
  
    
  // welcome
  welcome_operations();

  // Reset source
  show_reset_source();	   

  // Init code  
  
  
  while(1){
    // Only by Reset get out
    while(Sleep_counter_mode>=10){
      SLEEP();
    }
    //SLEEP();
    
    // update values
    auxKey=Key;
    Key=0;

    // Inactivity by 10 seconds
    if(auxKey!=0){
      Sleep_counter_mode=0;
    }

    // key to symbol
    key2symbol(&auxKey,&symbol);

    // colors
    if(symbol!='F'){
      commute_color(&color);
      color++;
      if(color==8) color=0;
    }
    
    // User operations
    if (symbol=='D'){
      BorraLCD();
      MensajeLCD_Var("Cleaning...");
      clear_symbols(&dig_1,&sym_ope,&dig_2,&sym_res);
      __delay_ms(1000);
      BorraLCD();
    }
    else if(dig_1=='F' && symbol!='F'){
      BorraLCD();
      if(is_sym_val_dig(valid_sym_dig,&symbol)){
	dig_1=symbol;
	sprintf(aux_view_lcd,"%u",dig_1);
	print_array_char(aux_view_lcd);
      }
      else{
	print_bad_dig();
	rewrite=1;
      }
    }
    else if(sym_ope=='F' && symbol!='F'){
      if(is_sym_val_ope(valid_sym_ope,&symbol)){
	sym_ope = symbol;
	sprintf(aux_view_lcd,"%c",sym_ope);
	print_array_char(aux_view_lcd);
      }
      else{
	print_bad_ope();
	rewrite=1;
      }
    }
    else if(dig_2=='F' && symbol!='F' && sym_ope!='!'){
      if(is_sym_val_dig(valid_sym_dig,&symbol)){
	dig_2=symbol;
	sprintf(aux_view_lcd,"%u",dig_2);
	print_array_char(aux_view_lcd);
      }
      else{
	print_bad_dig();
	rewrite=1;
      }

    }
    else if(sym_res=='F' && symbol!='F'){
      if(is_sym_val_res(&valid_sym_res,&symbol)){
	sym_res=symbol;
	sprintf(aux_view_lcd,"%c",sym_res);
	print_array_char(aux_view_lcd);
	calculate_and_show_result(&dig_1,&dig_2,&sym_ope,aux_view_lcd);
	clear_symbols(&dig_1,&sym_ope,&dig_2,&sym_res);
      }
      else{
	print_bad_res();
	rewrite=1;
      }
      
    }

    // Print again after of bad input
    if(rewrite){
      if(dig_1!='F'){
	sprintf(aux_view_lcd,"%u",dig_1);
	print_array_char(aux_view_lcd);
     
      }
      if(sym_ope!='F'){
	sprintf(aux_view_lcd,"%c",sym_ope);
	print_array_char(aux_view_lcd);
      }
      if(dig_2!='F' && sym_ope!='!'){
	sprintf(aux_view_lcd,"%u",dig_2);
	print_array_char(aux_view_lcd);
      }
      if(sym_res!='F'){
	sprintf(aux_view_lcd,"%c",sym_res);
	print_array_char(aux_view_lcd);
	
      }
      rewrite=0;
    }

    
    
  }    
}
// Thanks to: https://www.tutorialspoint.com/cprogramming/c_function_call_by_reference.htm
unsigned char is_sym_val_dig(unsigned char *arr, unsigned char *sym){
  unsigned char counter=0;
  while(arr[counter]!='F'){
    if(arr[counter]==*sym) return 1;
    counter++;
  }
  return 0;
}
unsigned char is_sym_val_ope(unsigned char *arr, unsigned char *sym){
  unsigned char counter=0;
  while(arr[counter]!='\0'){
    if(arr[counter]==*sym) return 1;
    counter++;
  }
  return 0;
}
unsigned char is_sym_val_res(unsigned char *sym_res, unsigned char *sym){
  if(*sym_res==*sym) return 1;
  return 0;
}

 
void key2symbol(unsigned char *key, unsigned char *symbol){
  switch(*key){
  case 1: *symbol=1;
    break;
  case 2: *symbol=2;
    break;
  case 3: *symbol=3;
    break;
  case 4:{
    if (Mode!=0) *symbol='^';
    else *symbol='+';
    break;
  }
    
  case 5: *symbol=4;
    break;
  case 6: *symbol=5;
    break;
  case 7: *symbol=6;
    break;
  case 8: {
    if(Mode!=0) *symbol='!';
    else  *symbol='-';
    break;
  }
  case 9: *symbol=7;
    break;
  case 10: *symbol=8;
    break;
  case 11: *symbol=9;
    break;
  case 12: *symbol='=';
    break;
  case 13: *symbol='X';
    break;
  case 14: *symbol=0;
    break;
  case 15: *symbol='/';
    break;
  case 16: *symbol='D';
    break;
  default: *symbol='F';
    break;
  }
  return;
}


void aux_search_key(void){
  // Thanks to https://www.tutorialspoint.com/cprogramming/c_array_of_pointers.htm
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
    __delay_ms(10); 		//stabilize LATB 
    
    if(RB4==0){Key=numbers_key[i*4];break;} 
    if(RB5==0) {Key=numbers_key[i*4+1];break;}
    if(RB6==0) {Key=numbers_key[i*4+2];break;}
    if(RB7==0) {
      Key=numbers_key[i*4+3];
      unsigned char contador = 0;
      while(RB7==0 && i==3){
	__delay_ms(10);
	contador++;
	if (contador==120){
	  Mode=~Mode;
	  BorraLCD();
	  MensajeLCD_Var("Changing mode...");
	  __delay_ms(1000);
	  BorraLCD();
	  break;
	}
      }
      break;
    
    }
  }

  return;

}

void __interrupt(low_priority) ISR_low(void){
  
  if(RBIF==1){
    Key=0;
    unsigned char aux_PORTB=PORTB;
    // Falling edge discrimination
    if((aux_PORTB & 0b11110000)!=0b11110000){
      aux_search_key();
      LATB=0b11110000;
    }
    __delay_ms(100);
    RBIF=0;
  }
  return;
}

void __interrupt(high_priority) ISR_high(void){
  if(TMR0IF==1){
    TMR0=Preload;
    TMR0IF=0;
    LEDFRE=~LEDFRE;

    // Increasing sleep counter
    Sleep_counter_mode++;
  }
  return;
}

void show_reset_source(void){
  BorraLCD();
  if(POR==0){
    MensajeLCD_Var("Reset source: POR");
    POR=1;
  }
  else{
    MensajeLCD_Var("Reset source: MCLR");
  }
  __delay_ms(3000);
  BorraLCD();
  return;
}

void init_configuration(void){
  // Sleep
  IDLEN=1;
//  SCS1=1;
//  SCS0=1;
  
  // RB0:RB4 and RE0:RE2 as i/o digital
  ADCON1=10;

  // RGB in RE0:RE2
  TRISE=0b11111000;
  LATE=1;
  
  // Led in RC2
  TRISC=0b11111011;
  LATC=0;
  
  /* LCD configuration */
  TRISD=0;			/*PortD as output */
  LATD=0;			/* PortD init in 0 logic */
  
  ConfiguraLCD(4);
  InicializaLCD();

  /* Keyboard configuration */
  TRISB=0b11110000;	     /*  RB7:RB4 as input and RB3:RB0 as output */
  LATB=0b00000000;	     /* RB3:RB0 in 0 logic */
  RBPU=0;			/*  PORTB Pull-up Enable bit, 0 is enable pullup resistence */

  RBIF=0; 			// RB Port Change Interrupt Flag bit, 0:  None of the RB7:RB4 pins have changed state
  RBIP=0;			// Low priority
  RBIE=1;			// RB Port Change Interrupt Enable bit, 1: Enables the RB port change interrupt
  
  
  // TM0 configuration

  // T0CON register
  // bit 7 - TMR0ON: 0-1, stop Timer()-enables Timer()
  // bit 6 - T08BIT: 0, 16 bits
  // bit 5 - T0CS: 0, Fbus
  // bit 4 - T0SE: 0, Increment on low-to-high transition on T0CKI pin ?
  // bit 3 - PSA: 0, Assigned prescaler
  // bit 2:0 - T0PS2:T0PS0: 001,1:4 Prescale value

  T0CON=0b00000001;

  TMR0=Preload; 			//preload
  // INTCON
  // TMR0IF: 0, TMR0 register did not overflow
  // TMR0IE: 1, Enables the TMR0 overflow interrupt

  TMR0IF=0;			
  TMR0IE=1;
  TMR0IP=1;			// 1, High priority
  
  IPEN=1;			// Enable priority levels on interrupts
  PEIE=1;			// Enable interruption bit
  GIE=1;  			// Global Interrupt Enable bit, 1: = Enables all unmasked interrupts
  // Enable timer
  TMR0ON=1; 
  
  /* General configuration */
  __delay_ms(1000);		/* LCD stabilize time (1000 ms) and pull-up resistences (100 ms)*/
  return;
}

void welcome_operations(void){
  /* Welcome message */
  BorraLCD();
  MensajeLCD_Var("Hola Mundo");
  DireccionaLCD(0xC0); 
  MensajeLCD_Var("Dios es bueno");

  __delay_ms(1000);
  BorraLCD();
  return;
}

void print_array_char(char *arr){
  // thanks to: https://www.geeksforgeeks.org/what-is-the-best-way-in-c-to-convert-a-number-to-a-string/
  unsigned char counter=0;
  while (arr[counter]!='\0') {
    EscribeLCD_c(arr[counter]);
    counter++;
  }
  return;
}

void print_bad_dig(void){
  BorraLCD();
  MensajeLCD_Var("Insert a digit");
  __delay_ms(1000);
  BorraLCD();
  return;
}

void print_bad_ope(void){
  BorraLCD();
  MensajeLCD_Var("Insert an operation");
  __delay_ms(1000);
  BorraLCD();
  return;
}

void print_bad_res(void){
  BorraLCD();
  MensajeLCD_Var("Insert '=' symbol");
  __delay_ms(1000);
  BorraLCD();
  return;
}

void calculate_and_show_result(unsigned char *dig_1, unsigned char *dig_2, unsigned char *sym_ope, char *aux_view_lcd){

  unsigned long long int aux_uns_long_long_int=0;
  
  switch(*sym_ope){
  case '+':{
    unsigned char res_sum=*dig_1+*dig_2;
    sprintf(aux_view_lcd, "%u",res_sum);
    print_array_char(aux_view_lcd);
    break;
  }
    
  case '-':{
    int res_res=*dig_1-*dig_2;
    sprintf(aux_view_lcd, "%i",res_res);
    print_array_char(aux_view_lcd);
    break;
  }
  case 'X':{
    unsigned char res_mul=(*dig_1)*(*dig_2);
    sprintf(aux_view_lcd, "%u",res_mul);
    print_array_char(aux_view_lcd);
    break; 
  }
  case '/':{
    float res_div;
    float aux_res_div;
    if(*dig_1==0 && *dig_2==0){
      sprintf(aux_view_lcd, "%s","Ind");
      print_array_char(aux_view_lcd);
    }
    else if(*dig_2==0){
      sprintf(aux_view_lcd, "%s","Inf");
      print_array_char(aux_view_lcd);
    }
    else{
      res_div = (float)(*dig_1)/(*dig_2);
      // Check if division is exactly
      aux_res_div = res_div-(int)res_div;

      if(1E-6>aux_res_div){
	sprintf(aux_view_lcd, "%.0f",res_div);	
      }
      else{
	sprintf(aux_view_lcd, "%.3f",res_div);
      }
      
      print_array_char(aux_view_lcd);
    }

    break;
  }
  case '!': {

    aux_uns_long_long_int = my_factorial(*dig_1);
    sprintf(aux_view_lcd, "%llu",aux_uns_long_long_int);
    print_array_char(aux_view_lcd);
	
      
    break;
  }
  case '^':{
    if(*dig_1==0 && *dig_2 == 0){
      sprintf(aux_view_lcd, "%s","Ind");
      print_array_char(aux_view_lcd);
    }
    else if(*dig_1==0){
      sprintf(aux_view_lcd, "%c",'0');
      print_array_char(aux_view_lcd);
    }
    else{
      aux_uns_long_long_int  = my_pow(*dig_1, *dig_2);
      sprintf(aux_view_lcd, "%llu",aux_uns_long_long_int);
      print_array_char(aux_view_lcd);
    }
    
    break;
  }
    
  default: {
    BorraLCD();
    MensajeLCD_Var("Error operation");
    __delay_ms(1000);
    BorraLCD();

    break;
  }  
  }
  return;
}



void commute_color(unsigned char *color){

  switch(*color){
  case 0:{
    // Black
    RGB_R=1;
    RGB_G=1;
    RGB_B=1;
    break;
  }
  case 1: {
    // Magenta
    
    RGB_R=0;
    RGB_G=1;
    RGB_B=0;
    break;
  }
  case 2: {
    // blue 
    RGB_R=1;
    RGB_G=1;
    RGB_B=0;
    break;
  }
  case 3: {
    // cyan
    RGB_R=1;
    RGB_G=0;
    RGB_B=0;
    break;
  }
  case 4: {
    // green
    RGB_R=1;
    RGB_G=0;
    RGB_B=1;
    break;
  }
  case 5: {
    // Yellow
    RGB_R=0;
    RGB_G=0;
    RGB_B=1;
    break;
  }
  case 6: {
    // red
    RGB_R=0;
    RGB_G=1;
    RGB_B=1;
    break;
  }
  case 7: {
    // white
    RGB_R=0;
    RGB_G=0;
    RGB_B=0;
    break;
  }
  default:{
    // Black
    RGB_R=1;
    RGB_G=1;
    RGB_B=1;
    break;
  }
  }
  
  return;
}

void clear_symbols(unsigned char *dig_1, unsigned char *sym_ope, unsigned char *dig_2, unsigned char *sym_res){
  *dig_1 = 'F';
  *sym_ope = 'F';
  *dig_2 = 'F';
  *sym_res = 'F';
  return;
}

unsigned long long int my_pow(unsigned char x, unsigned char n){
  unsigned long long int aux=x;
  for(unsigned char i=1;i<n;i++){
    aux=x*aux;
  }
  return aux;
}

unsigned long long int my_factorial(unsigned char x){
  unsigned long long int aux=1;
  for(unsigned char i=2;i<=x;i++){
    aux=aux*i;
  }
  return aux;
}
