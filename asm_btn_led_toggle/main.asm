;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
LedInit:   mov.b  #0F7h, &P1DIR  ; P1.3 as input, P1.0 and
                                 ; other pins as output
           bis.b  #01h, &P1OUT   ; turn on led at P1. ...
;-----------------------------------------------------------
BtnSetup:  bis.b  #08h, &P1REN   ; pull-up resistor at P1.3
           bis.b  #08h, &P1OUT   ; set btn as normally closed
           bis.b  #08h, &P1IE    ; enable port1 interrupt
           bic.b  #08h, &P1SEL   ; interrup flag on low-to-high
;------------------------------------------------------------
MainLoop:  bit.b  #BIT0, &P1IFG  ; test for button state
           jz     MainLoop       ; if not pressed, test it again
           call   #ToggleLed     ; if pressed, go to subroutine
           jmp    MainLoop       ; poll it again

;-------------------------------------------------------------
;   Toggle Led subroutine
;-------------------------------------------------------------
ToggleLed: bic.b  #BIT0, &P1IFG ; clear interrupt flag
           xor.b  #BIT0, &P1OUT
           ret                  ; return to main loop

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
            .end
