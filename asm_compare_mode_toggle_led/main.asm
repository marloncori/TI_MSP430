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
Clk_Config:
			mov.w	#LFXT1S_2, &BCSCTL3    ; a 12 KHz VLO as ACLK source

Led_Init:
			mov.b	#41h, &P1DIR
			mov.b   #01h, &P1OUT
Int_Enable:
			mov.w	#CCIE, &TACCTL0			; TACCR0 interrupt enabled

Upper_Limit:
			mov.w	#999d, TACCR0			; Timer A counts up to 1000

TA0_Config:
			mov.w	#TASSEL_1+MC_1, &TACTL	; ACLK as source, upmode (counts up to TACCR0 value)

Interrupts:
			bis.w	#LPM1+GIE, SR			; enable global interrupts, enter power mode 1

;-------------------------------------------------------------------------------
TA0_ISR  ; timer A zero interrupt service routine here - toggle P1.0 pin
;-------------------------------------------------------------------------------
			xor.b	#41h, P1OUT
			reti
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
            
            .sect   TIMER0_A0_VECTOR
            .short  TA0_ISR

            .end
