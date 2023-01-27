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
StrConfig:	mov.b	#'H', &0200h			; this is where memory for gp use starts
			mov.b	#'e', &0201h
            mov.b	#'l', &0202h
            mov.b	#'l', &0203h
            mov.b	#'o', &0204h
            mov.b	#' ', &0205h
            mov.b	#'W', &0206h
            mov.b	#'o', &0207h
            mov.b	#'r', &0208h
            mov.b	#'l', &0209h
            mov.b	#'d', &020Ah

            mov.b	#0Ah, &020Bh			; new line
            mov.b	#0Dh, &020Ch            ; cursor at the beginning of line
            mov.b	#0h, &020Dh				; null character

ClockInit:  mov.b	&CALBC1_1MHZ, &BCSCTL1	; adjust internal clock
            mov.b	&CALDCO_1MHZ, &DCOCTL

LedSetup:	mov.b	#6h, &P1SEL
            mov.b	#6h, &P1SEL2
            bis.b	#08h, &P1IE
            bis.b	#08h, &P1IES
            clr.b	P1IFG

USART_Cfg:  bis.b	#UCSWRST+UCSSEL_2, &UCA0CTL1	; adjust USART mode
													; enable SW reset, use SMCLk
			mov.b	#68h, &UCA0BR0					; low bit of UCBRx is 104
			mov.b	#0h, &UCA0BR1					; high bit of UCBRx is 0
			mov.b	#UCBRS_1, &UCA0MCTL
			; second modulation stage select is 1, baud rate = 9600
			bic.b	#UCSWRST, &UCA0CTL1				; clear SW reset, resume operation

Interrupts:	bis.w	#GIE+LPM4, SR

Transmit:	mov.w	#0200h, R5

Check:		bit.b	#UCA0TXIFG, &IFG2				; ensure transmit interrupt flag is set
			jeq		Check							; if not set jmp to Check label

			mov.b	@R5, R6
			tst.b	R6								; check if new char equals null char
			jeq		Finish							; if it is, jmp to Finish label
			mov.b	@R5+, &UCA0TXBUF				; if not, load TX buffer with string chars
			jmp 	Check							; jump to Check label to send a new char

Finish:		ret
;---------------------------------------
P1_ISR
;---------------------------------------
			call	#Transmit
			clr.b	P1IFG
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
            
            .sect	PORT1_VECTOR
            .short 	P1_ISR
            .end
