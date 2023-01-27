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
SetupP3: 	bis.b 	#030h,&P3SEL 			; P3.4,5 = USART0 TXD/RXD

SetupBC: 	bis.b 	#XTS,&BCSCTL1 			; LFXT1 = HF XTAL

SetupOsc: 	bic.b 	#OFIFG,&IFG1 			; Clear OSC fault flag
			mov.w 	#0FFh,R15 				; R15 = Delay

SetupOsc1: 	dec.w 	R15 					; Addnl. delay to ensure start
			jnz 	SetupOsc1
			bit.b 	#OFIFG,&IFG1 			; OSC fault flag set?
			jnz 	SetupOsc 				; OSC Fault, clear flag again
			bis.b 	#SELM_3,&BCSCTL2 		; MCLK = LFXT1

SetupUART0:	bis.b	#UTXE0+URXE0,&ME1 		; Enable USART0 TXD/RXD
			bis.b 	#CHAR,&UCTL0 			; 8-bit characters
			mov.b 	#SSEL0,&UTCTL0 			; UCLK = ACLK
			mov.b 	#045h,&UBR00 			; 8 MHz 115200
			mov.b 	#000h,&UBR10 			; 8 MHz 115200
			mov.b 	#000h,&UMCTL0 			; 8 MHz no modulation 115200
			bic.b 	#SWRST,&UCTL0 			; **Initialize USART FSM **
;
Mainloop: 	bis.b 	#CPUOFF+GIE,SR 			; Enter LPM0, intrpts enabled
			nop 							; Needed only for debugger
;
;-------------------------------------------------------------------------
USART0RX_ISR; Confirm TX buffer is ready, then Echo back RXed character
;-------------------------------------------------------------------------
TX1:		bit.b 	#UTXIFG0,&IFG1 			; USART0 TX buffer ready?
				jz 	TX1 					; Jump is TX buffer not ready
			mov.b   &RXBUF0,&TXBUF0 		; TX -> RXed character
 			reti ;;

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
            
			.sect 	USART0RX_VECTOR
			.short	USART0RX_ISR

			.end
