;Librerias
include p18f4550.inc

;Configuración
;Variables
var1 equ 0x0 ;variable 1
auxR equ 0x1 ;guarda resultados
var2 equ 0x2 ;variable 2
var3 equ 0x3 ;variable 3
;Código
Inicio
    ; var1<-9_10 y se comprueba su máximo valor
        ; var1<-9_10
    movlw .9
    movwf var1
        ; var1<-255_10
    movlw .255
    movwf var1
       ; var1<-256_10
    movlw .256
    movwf var1
       ; var1<257_10
    movlw .257
    movwf var1
    
    ; Inicio de las operaciones
    ; var1<-7_10
    ; auxR=var1+3_10
	; var1<-7_10
    movlw .7
    movwf var1
 
        ;WREG=WREG+3_10
    addlw .3
	;auxR<-WREG
    movwf auxR
    ; var1<-8_10,var2<-10_10
    ; auxR=var1+var2
	;var1<-8_10
    movlw .8
    movwf var1
	;var2<-10
    movlw .10
    movwf var2
	;WREG+=var1
    addwf var1,0
	;auxR<-WREG
    movwf auxR
    ; var1<-5_10
    ; auxR=9_10-var1
	;var1<-5
    movlw .5
    movwf var1
	;WREG=9-WREG
    sublw .9
    movwf auxR
    ; var1<-6_10, var2<-4_10
    ; auxR=var2-var1
	;var2<-4_10
    movlw .4
    movwf var2
	;var1<-6_10
    movlw .6
    movwf var1
	;WREG=var2-WREG
    subwf var2,0
	;auxR=WREG
    movwf auxR
    
    ; var1<-12
    ; auxR=~var1 (complemento a 1 de var1)
	; var1<-12
    movlw .12
    movwf var1
	;WREG=~var1
    comf var1,0
	;var1=WREG
    movwf auxR
    
    ; var1<-12
    ; auxR=~var1+1 (complemento a 2 de var1)
	; var1<-12
    movlw .12
    movwf var1
	; var1=~var1+1
    negf var1
	;auxR=~var1+1, var1=WREF
    movff var1,auxR
    movwf var1
    
    ; var1=35_10
    ; auxR=var1 | 7
	; var1=35_10
    movlw .35
    movwf var1
	; WREG=WREG|7_10
    iorlw .7
	; auxR=WREG
    movwf auxR

    ; var1=20,var2=56
    ; auxR=var1 | var2
	;var1=20
    movlw .20
    movwf var1 
	;var=56
    movlw .56
    movwf var2
	; WREG=WREG|var1
    iorwf var1,0
    movwf auxR
    
    ; var1=62_10
    ; auxR=var1&15_10
	;var1=62_10
    movlw .62
    movwf var1
	; WREG=WREG&15_10
    andlw .15
    movwf auxR
    
    ; var1=100, var2=45
    ; auxR=var1&var2
	; var1=100
    movlw .100
    movwf var1
	; var2=45
    movlw .45
    movwf var2
	; WREG=WREG & var1
    andwf var1,0
    movwf auxR
   
    ; var1=120
    ; auxR=var1 xor 1
	; var1=120
    movlw .120
    movwf var1
	; WREG = WREG xor 1
    xorlw .1
	; auxR = WREF
    movwf auxR
    
    ; var1=17,var2=90
    ; auxR=var1 xor var2
	; var1=17
    movlw .17
    movwf var1
	; var2=90
    movlw .90
    movwf var2
	;WREG=WREG xor var1
    xorwf var1,0
	;auxR=WREG
    movwf auxR
    
    ; var1=25,var2=40,var3=103
    ; auxR=(var1 OR var2) and (var3 xor 0xd0)
	; var1=25
    movlw .25
    movwf var1
	; var1=40
    movlw .40
    movwf var2
	; var1=103
    movlw .103
    movwf var3
	; WREG= var3 xor 0xd0 
    xorlw 0xd0
	; auxR=WREG
    movwf auxR
    
	; WREG = var1
    movf var1,0
	; WREG= var1 OR var2
    iorwf var2,0
	
	; WREG = (var1 OR var2) & (var3 xor 0xd0)
    andwf auxR,0
	; auxR = WREG
    movwf auxR
    
    ; var1=18_10,var2=60_10, var3=16_10
    ; auxR=(var1 + var2) - (var3-0b11010)
    ; 0b11010 = 26_10
    ; auxR=88
    ; var1=18
    movlw .18
    movwf var1
	; var2=60
    movlw .60
    movwf var2
	; var3=16
    movlw .16
    movwf var3
	; WREG= b'11010'
    movlw b'11010'
	; WREG=var3-WREG
    subwf var3,0
	; auxR=WREG
    movwf auxR
    
	;WREG=var1
    movf var1,0
	;WREG=var1+var2
    addwf var2,0
	;WREG=(var3-0b11010)-(var1 + var2)  
    subwf auxR,0
	;auxR=(var3-0b11010)-(var1 + var2)
    movwf auxR
	;auxR=(var1 + var2)-(var3-0b11010)
    negf auxR
    
end