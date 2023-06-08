//Fosc=1MHz
//BaudRate 9600 bps
#include<xc.h>
#define _XTAL_FREQ 1000000

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF


void Transmitir(unsigned char);
unsigned char BufferR='a';

void interrupt ISR(void);

unsigned char Recibir(void);

void main(void){
    //unsigned char BufferR;
    TXSTA=0b00100100;
    RCSTA=0b10010000;        
    BAUDCON=0b00001000;        
    SPBRG=25;               //9600 bps
    
    // Receive USART
    RCIE=1; // Enable reice USART interruption
    //RCIP=1 // 1->High, 0->Low priority
    
    PEIE=1; // Periferical interruption enable
    GIE=1; // Global Interruption =1;
    
    char i =255;
    while(1){     
        if(i+48>255) i=255;
//        Transmitir('H');
//        Transmitir('o');
//        Transmitir('l');
//        Transmitir('a');
//        Transmitir(' ');
//        Transmitir('m');
//        Transmitir('u');
//        Transmitir('n');
//        Transmitir('d');
//        Transmitir('o');
//        Transmitir(' ');
//        Transmitir(++i+48);
        //Transmitir(BufferR);
        Transmitir('J');
        Transmitir(BufferR);
        Transmitir('\n');
        
        if(BufferR=='3'){
            Transmitir('O');
            Transmitir('K');
            Transmitir('\n');
        }
        
        CREN=1; // activate again to receive by rs232
        __delay_ms(3000);
        //BufferR=Recibir();       
    }
}

unsigned char Recibir(void){
    while(RCIF==0);
    return RCREG;
}

void Transmitir(unsigned char BufferT){
    while(TRMT==0);
    TXREG=BufferT;
}

void interrupt ISR(void){
    if(RCIF==1){
        
        BufferR=RCREG;
        CREN=0; // Is necessary stop to only receive first byte 
    }
    
    return;
}