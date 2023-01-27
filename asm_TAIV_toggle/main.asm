;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;  TAVI - "timer A interrupt vector"
;  Here the red LED toggles when an overflow occurs. This happens every second
; (based on the parameter settings). Here, the capture/compare block is not used.
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
Setup:		mov.b	#01h, &P1DIR
			mov.b	#01h, &P1OUT

			mov.w	#TASSEL_2+ID_3+MC_3+TAIE, &TACTL ; TASSEL_0 = TACLK, TASSEL_1 = ACLK
					;TASSEL_2 = SMCLK,
					; modes for the divider for the input clock:
					; ID_0 = /1, ID_1 = /2, ID_2 = /4, ID_3 = /8,
					; mode control options: MC_1 up mode, counts up to TACCR0 set value
					; MC_2: continuous modde, counts up to 0FFFFh
					; MC_3: up/down mode - counts till TACCR0 set value and then down to zero.
					; TAIE - timer A interrupt enable in the time A control register

													; timer overflow interrupt enabled
			mov.w	#62500d, &TACCR0				; add offset to TACCR0,
			; since I selected up/down mode the counting will be 0->62500 and 62500->0 (125000)
			; max count is 0FFFh, i.e. 65535, so 62500 is 65535-3035

			bis.w	#GIE+LPM1, SR					; enable interrupts and enter LPM

;-----------------------------------------------------------------------------------------------
T0_A1_ISR	; timer zero A1 interrupt service routine, it toggles LED at P1.0 pin.
;-----------------------------------------------------------------------------------------------
			add.w	&TAIV, PC						; use PC for observing the
													;	timer A interrupt vector
			reti									; no interrupt pending
			reti									; TACCR0
			reti									; TACCR1
			reti									; reserved

			reti									; reserved
			xor.b	#01h, &P1OUT					; toggle P1.0
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
            
			.sect   TIMER0_A1_VECTOR
			.short	T0_A1_ISR
			.end
