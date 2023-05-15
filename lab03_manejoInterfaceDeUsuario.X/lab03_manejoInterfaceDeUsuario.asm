; Libraries
include p18f4550.inc

; Configuration
;CONFIG FOSC = INTOSC_EC ; Internal Oscillator 1 MHz
CONFIG FOSC = HS ;Crystal oscillator [4,48] MHz
CONFIG WDT = OFF ; Shutdown WatchDog Timer

; CPUDIV1:CPUDIV0 in datasheet but in mplab is: (Window -> PIC Memory view -> 
    ;Configuration bits)

;CONFIG CPUDIV = OSC1_PLL2 ; Postscaler external clock by /1
;CONFIG CPUDIV = OSC2_PLL3 ; Postscaler external clock by /2
;CONFIG CPUDIV = OSC3_PLL4 ; Postscaler external clock by /3
;CONFIG CPUDIV = OSC4_PLL6 ; Postscaler external clock by /4
    
; Variables
value equ 0x0 ; variable with number to view
table_index equ 0x1 ; get the code to color and number from tables
; delay variables
aux1 equ 0x2
aux2 equ 0x3
aux3 equ 0x4

 
; code
init
    ; OSCCON<6-4> <- 110_2 ; F int osc =4 MHz
;    bsf OSCCON,6
;    bsf OSCCON,5
;    bcf OSCCON,4
    
    movlw .15
    movwf ADCON1  ;RB0 to RB4, RA0 to RA4  and RE0 to RE2 as digital i/o
    clrf TRISD; Port D as output to seven segments
    clrf LATD; seven segments start as clear
    
    movlw b'11100011'
    movwf TRISB; RB2, RB3 y RB4 as analog outputs of RGB
    setf LATB; RGB start as black
    
    setf TRISA; RA0 ... RA2 as analog inputs
    
    

restart
    movlw .0
    movwf value
    
check_reset_button
    call delay_switch_debounce
    btfsc PORTA,2 ; PORTA,2 == 0 ? aux_restart : interrupter_inc_or_dec 
    goto interrupter_inc_or_dec
    goto aux_restart
    
aux_restart
    setf LATB; Black color                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    call delay_2_s
    goto restart
 
interrupter_inc_or_dec
    call delay_switch_debounce
    btfsc PORTA,1 ; PORTA,1 == 0 ? increase_value : decrement_value
    goto decrement_value
    goto increase_value
    
decrement_value    
    decf value,f

    check_value_is_negative_or_zero
	bn value_is_negative
	movlw .0
	cpfseq value ; value == 0 ? value_is_zero : show_value
	goto show_value

    value_is_zero
	movlw .6
	movwf value
	goto show_value

    value_is_negative
	movlw .1
	movwf value
	goto show_value

increase_value
    incf value,f
   
    check_value_is_seven
	movlw .7
	cpfseq value ; Value == 7 ? value_is_seven : show_value
	goto show_value

    value_is_seven
	movlw .1
	movwf value
	goto show_value
 
show_value
    
    movff value,table_index
    decf table_index,f ; First position is cero in table
    
    ; PCH <- 0x7 when PCL<-PCL+WREG
    movlw 0x7
    movwf PCLATH
    ;index in table is each two positions
    rlncf table_index,w ; w = 2*table_index
    call seven_segments_code_table
    
    movwf LATD ; setting code for number in seven segments
    
    ; PCH <- 0x8 when PCL<-PCL+WREG
    movlw 0x8
    movwf PCLATH
    ;index in table is each two positions
    rlncf table_index,w ; w = 2*table_index
    call color_code_table
    
    movwf LATB ; Setting color in RGB
    
button_delay
    call delay_switch_debounce
    btfsc PORTA,0 ; PORTA,0 == 0 ? button_delay_is_cero : button_delay_is_one 
    goto button_delay_is_one
     
button_delay_is_cero
    
    ;White Color
    clrf LATB
    call delay_5_s
    goto check_reset_button
    
button_delay_is_one
    call delay_200_ms
    goto check_reset_button
    

delay_5_s ;Exactly Fosc = 4 MHz
    movlw .185
    movwf aux1
    movlw .144
    movwf aux2
    movlw .62
    movwf aux3
aux_delay_5_s
    decfsz aux1,f
    goto aux_delay_5_s
    movlw .185
    movwf aux1
    decfsz aux2,f
    goto aux_delay_5_s
    movlw .144
    movwf aux2
    decfsz aux3,f
    goto aux_delay_5_s
    return
     
delay_2_s ; 1e-4 % absolute error Fosc = 4 MHz
    movlw .143
    movwf aux1
    movlw .192
    movwf aux2
    movlw .24
    movwf aux3
aux_delay_2_s
    decfsz aux1,f
    goto aux_delay_2_s
    movlw .143
    movwf aux1
    decfsz aux2,f
    goto aux_delay_2_s
    movlw .192
    movwf aux2
    decfsz aux3,f
    goto aux_delay_2_s
    return

delay_200_ms ; Exactly Fosc = 4 MHz
    movlw .64
    movwf aux1
    movlw .145
    movwf aux2
    movlw .7
    movwf aux3
aux_delay_200_ms
    decfsz aux1,f
    goto aux_delay_200_ms
    movlw .64
    movwf aux1
    decfsz aux2,f
    goto aux_delay_200_ms
    movlw .145
    movwf aux2
    decfsz aux3,f
    goto aux_delay_200_ms
    return

 ; delay 10 ms Exactly Fosc = 4 MHz
 delay_switch_debounce
    movlw .220
    movwf aux1
    movlw .5
    movwf aux2
    movlw .3
    movwf aux3
aux_delay_switch_debounce
    decfsz aux1,f
    goto aux_delay_switch_debounce
    movlw .220
    movwf aux1
    decfsz aux2,f
    goto aux_delay_switch_debounce
    movlw .5
    movwf aux2
    decfsz aux3,f
    goto aux_delay_switch_debounce
    return


ORG 0x0700
seven_segments_code_table
    addwf PCL,f
    ;       76543210
    retlw b'00000101' ; code to 1
    retlw b'11101110' ; code to 2
    retlw b'11101101' ; code to 3
    retlw b'10011101' ; code to 4
    retlw b'11111001' ; code to 5
    retlw b'11111011' ; code to 6

ORG 0x0800
color_code_table
    addwf PCL,f
    retlw b'11101011'; code to magenta
    retlw b'11111011' ; code to blue
    retlw b'11110011' ; code to cyan
    retlw b'11110111' ; code to green
    retlw b'11100111' ; code to yellow
    retlw b'11101111' ; code to red
    
end
    
    
