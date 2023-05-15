;Librerias
include p18f4550.inc

; Configuración
CONFIG FOSC = EC_EC ;Oscilador Externo
CONFIG WDT = OFF ; Apagar Perro Guardian

  
; Código
Main
    ; Configuración puertos
    clrf TRISD 
    setf LATD
Loop
    ;, bit2 -> Blue , bit1 -> Green y bit0 -> Red
    ; Negro b'00000 111'
    setf LATD 
    ; call Retardo
    
    ; Azul b'00000 011'
    bcf LATD,2
    ; call Retardo
    
    ; Cyan b'00000 001'
    bcf LATD,1
    ; call Retardo
    
    ; Verde b'00000 101'
    bsf LATD,2
    ; call Retardo
    
    ; Amarillo b'00000 100'
    bcf LATD,0
    ; call Retardo
    
    ; Blanco b'00000 000'
    bcf LATD,2 
    ; call Retardo
    
    ; Magenta b'00000 010'
    bsf LATD,1
    ; call Retardo
    
    ; Rojo b'00000 110'
    bsf LATD,2
    ; call Retardo
    goto Loop

    
end