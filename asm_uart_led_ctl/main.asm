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
ClkConfig:	mov.b	&CALBC1_1MHZ, &BCSCTL1
			mov.b	&CALDCO_1MHZ, &DCOCTL

LedSetup:	bis.b	#41h, P1DIR
			clr.b	P1OUT
			mov.b	#6h, P1SEL
			mov.b	#6h, P1SEL2

UART_Init:	bis.b	#UCSWRST+UCSSEL_2, &UCA0CTL1 ; enable SW reset, use SMCLk
			mov.b	#68h, &UCA0BR0				 ; low bit of UCBRx is one
			mov.b	#0h, &UCA0BR1				 ; high bit of UCBRx is zero
			mov.b	#UCBRS_1, &UCA0MCTL			 ; second modulation stage select is 1
												 ; baud rate is 9600
			bic.b	#UCSWRST, &UCA0CTL1			 ; clear SW reset, resume operation

Interrupts:	bis.b	#UCA0RXIE, &IE2				; enable RX interrupt
			bis.w	#GIE+LPM0, SR

;----------------------------------------------
USCIAB0RX_ISR
;----------------------------------------------
			cmp.b	#'r', &UCA0RXBUF			; check is received char is 'r'
			jne		Second						; if not, jmp to Second label
			mov.b	#01h, &P1OUT					; if it is, turn on red LED
			jmp		EndISR

Second:		cmp.b	#'g', &UCA0RXBUF			; check is received char is 'g'
			jne		Third
			mov.b	#40h, &P1OUT
			jmp		EndISR

Third:		mov.b	#00h, P1OUT					; if anything else, turn off all leds

EndISR:		reti
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

			.sect	USCIAB0RX_VECTOR
			.short	USCIAB0RX_ISR
			.end
