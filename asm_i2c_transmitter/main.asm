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
ToBeSent:	mov.w	#0000h, &200h
			mov.w	#00FAh, &202h
			mov.w	#01F4h, &204h
			mov.w	#02EEh, &206h
			mov.w	#03E8h, &208h

LedSetup:	bis.b	#20h, &P1DIR
			bis.b	#0C0h, &P1SEL
			bis.b	#0C0h, &P1SEL2
			bis.b	#08h, &P1IE
			bis.b	#08h, &P1IES
            clr.b	P1IFG

I2C_Setup: 	bis.b	#UCSWRST, &UCB0CTL1		; adjust I2C mode, enable SW reset
			mov.b	#UCMST+UCMODE_3+UCSYNC, &UCB0CTL0
			; clock mode 3, msb first, 8-bit SPI master, four pin mode
			mov.b	#UCSSEL_2+UCSWRST, &UCB0CTL1 ; use SMCLK, keep SW reset
			mov.b	#0Ch, &UCB0BR0			; low bit of UCBRx is 2
			mov.b	#00h, &UCB0BR1			; high bit of UCBRx is zero
			mov.w	#48h, &UCB0I2CSA		; slave address
			; fSCL = SMCLK/12 --> 100KHz
			bic.b	#UCSWRST, &UCB0CTL1		; clear SW reset, resume operation
			bis.b	#UCB0TXIE, &IE2			; enable RX interrupt
			mov.w	#0200h, R5				; write the start address of Tx data into R5
			mov.w	#0h, R6
			clr		R10						; clear r10 used for checking which element
											; is transmitted
LedUpdate:	bic.b	#20h, &P1OUT			; reset slave
			bis.b	#20h, &P1OUT

Interrupt:	bis.w	#GIE, SR

MainLoop:	mov.w	#2h, R7					; load R7 with two bytes

StopCondition:
			bit.b	#UCTXSTP, UCB0CTL1
			jne		StopCondition
			cmp.w	#1h, R6
			jmp		MainLoop

I2C_Init:	bis.b	#UCTR+UCTXSTT, &UCB0CTL1 ; I2C transmitter, start condition
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
			mov.b	R8, &UCB0TXBUF			; load tx buffer with high byte of incoming elements

OddByte:	cmp.w	#1h, R7					; check if R7 equals 1
			jne		DecByteNum
			mov.w	@R5+, R9
			mov.b	R9, &UCB0TXBUF			; load tx buffer with low byte of incoming elements
			inc.w	R10						; inc R10 to indicate that one element is sent

DecByteNum: dec.w	R7						; decrease byte number by 1
			jmp		EndISR

AllTransmitted:
			cmp.w	#5h, R10				; check if all five elements are sent
			jne		SendStop
			sub.w	#10h, R5				; if it is 5, reload PTXdata pointer with
											; address of first element
			clr		R10

SendStop:	clr		R6
			bis.b	#UCTXSTP, &UCB0CTL1		; I2C stop condition
			bic.b	#UCB0TXIFG, &IFG2		; clear USCI_B0 interrupt flag
			bic.w	#LPM0, 0(SP)

EndISR:		reti

;--------------------------------------------
P1_ISR
;--------------------------------------------
			mov.w	#1h, R6
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
            
            .sect USCIAB0TX_VECTOR
            .short USCIAB0TX_ISR
            .sect PORT1_VECTOR
            .short P1_ISR
            .end
