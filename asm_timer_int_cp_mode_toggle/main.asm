;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; toggle led using the timer interruptin compare mode
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
			mov.w	#LFXT1S_2, &BCSCTL3		; 12 KHz as ACLK source
			mov.b	#41h, &P1DIR			; basic clock select control register
			mov.b	#01h, &P1OUT
											; bit4 of timer A cap/cmp control 0 register
			mov.w	#CCIE, &TACCTL0			; TACCR0 capture/compare interrupt enabled
			mov.w	#999d, &TACCR0			; TACCR0 counts up to 1000
			mov.w	#TASSEL_1+MC_1, &TACTL	; 0:1 -> ACLK, 1:0 SMCLK, 11 INCLK/TBCLK, upmode
											; TASSEL is bit9 and 8 of the Timer A cntl reg
			; other possible modes are: 1:0 conitinous mode, timer counter up to 0FFFFh
			; 1:1 for up/down mode, counts upt to TACCR0 value then down to 0000h

			bis.w	#LPM1+GIE, SR			; low power mode 1, enagle global interrupts

;-------------------------------------------------------------------------------
; TIMER 0 INTERRUPT SERVICE ROUTINE
;-------------------------------------------------------------------------------
TA0_ISR
			xor.b	#41h, &P1OUT
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
            
            .sect	TIMER0_A0_VECTOR
            .short	TA0_ISR
            .end
