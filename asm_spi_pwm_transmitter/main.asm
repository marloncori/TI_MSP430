;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; SPI PWM application in four-pin mode, the master transmitter code in
; assembly language (two LaunchPads are used)
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
ToBeSent:	mov.w	#0000h, &200h
			mov.w	#00FAh, &202h
			mov.w	#01F4h, &204h
			mov.w	#02EEh, &206h
			mov.w	#03E8h, &208h

LedSetup:	bis.b	#21h, &P1DIR
			bis.b	#01, &P1OUT
			bis.b	#16h, &P1SEL
			bis.b	#16h, &P1SEL2
			bis.b	#08h, &P1IE
			bis.b	#08h, &P1IES
            clr.b	P1IFG

USART_Init: bis.b	#UCSWRST, &UCA0CTL1		; enable SW reset
			bis.b	#UCCKPH+UCMSB+UCMST+UCSYNC, &UCA0CTL0
			; clock mode 0, msb first, 8-bit SPI master, four pin mode
			mov.b	#UCSSEL_2+UCSWRST, &UCA0CTL1 ; use SMCLK, keep SW reset
			mov.b	#02h, &UCA0BR0			; low bit of UCBRx is 2
			mov.b	#00h, &UCA0BR1			; high bit of UCBRx is zero
			; fSCL = SMCLK/12 --> 100KHz
			clr.b	UCA0MCTL				; no modulation
			bic.b	#UCSWRST, &UCA0CTL1		; clear SW reset, resume operation
			bis.b	#UCA0RXIE, &IE2			; enable RX interrupt
			mov.w	#0200h, R5				; write the start address of Tx data into R5
			clr		R10						; clear r10 used for checking which element
											; is transmitted
LedUpdate:	bic.b	#20h, &P1OUT			; reset slave
			bis.b	#20h, &P1OUT

Interrupt:	bis.w	#GIE, SR

MainLoop:	mov.w	#2, R7
			bis.w	#LPM0, SR
			jmp		MainLoop

;-------------------------------------------
USCIAB0TX_ISR
;-------------------------------------------
			tst.w	R7						; test if R7 is zero
			jeq		AllTransmitted			; if it is, jmp to AllBytesTransmitted
			bit.w	#1h, R7					; check if R7 is odd
			jne		OddByte					; if it is, jmp to OddByte
			mov.w	@R5, R8					; write the incoming array element to R8
			swpb	R8
			mov.b	R8, &UCA0TXBUF			; load tx buffer with high byte of incoming elements

OddByte:	cmp.w	#1h, R7					; check if R7 equals 1
			jne		DecByteNum
			mov.w	@R5+, R9
			mov.b	R9, &UCA0TXBUF			; load tx buffer with low byte of incoming elements
			inc.w	R10						; inc R10 to indicate that one element is sent

DecByteNum: dec.w	R7						; decrease byte number by 1
			jmp		EndISR

AllTransmitted:
			cmp.w	#5h, R10				; check if all five elements are sent
			jne		DisableTX
			sub.w	#10h, R5				; if it is 5, reload PTXdata pointer with
											; address of first element
			clr		R10

DisableTX:	bic.b	#UCA0TXIE, &IE2			; disable TX interrupt

EndISR:		reti

;-------------------------------------------
USCIAB0RX_ISR 								; SPI slave receive ISR
;-------------------------------------------
			mov.b	UCA0RXBUF, R6			; copy received data from slave to R6
			tst.w	R7						; check if R7 is zero
			jne		EndISR2					; if not, jump to EndISR2 label
			bic.w	#LPM0, 0(SP)
			bis.b	#01h, &P1OUT			; exit LPM0 and deactivate slave

EndISR2:	reti

;-------------------------------------------
P1_ISR
;-------------------------------------------
			bic.b	#01h, P1OUT				; activate slave
			bis.b	#UCA0TXIE, &IE2			; enable TX interrupt
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
            
            .sect	USCIAB0TX_VECTOR
            .short	USCIAB0TX_ISR
            .sect	USCIAB0RX_VECTOR
            .short	USCIAB0RX_ISR
			.sect	PORT1_VECTOR
			.short	P1_ISR
			.end
