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
			mov.w	#LFXT1S_2, &BCSCTL3		; 12 KHz as ACLK source
			mov.b	#41h, &P1DIR
			mov.b	#01h, &P1OUT

			mov.w	#CCIE, &TACCTL0			; TACCR0 interrupt enabled
		    mov.w	#CCIE, &TA1CCTL0		; TA1CCR0 interrupt enabled
			mov.w	#62500, &TACCR0			; TACCR0 counts up to 625000
		 	mov.w	#6000d, &TA1CCR0		; TA1CCR0 counts up to 6000
			mov.w	#TASSEL_2+MC_2+ID_3+TACLR, &TACTL	; ACLK, upmode
			mov.w	#TASSEL_1+MC_3+ID_3+TACLR, &TA1CTL	; ACLK, upmode

			bis.w	#LPM1+GIE, SR			; low power mode 1, enagle global interrupts

;-------------------------------------------------------------------------------
; timerA zero and timerA one interrupt service routine
;-------------------------------------------------------------------------------
TA0_ISR
			xor.b	#04h, &P1OUT
			reti
                                            
TA1_ISR
			xor.b	#01h, &P1OUT
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

			.sect   TIMER1_A0_VECTOR
			.short  TA1_ISR
			.end
