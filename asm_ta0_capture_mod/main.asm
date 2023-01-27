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
Config:		mov.w	#LFXT1S_2, &BCSCTL3      ; 12 KHz VLO as ACLK source
			mov.b	#02h, &P1SEL
			mov.w	#CAP+CM_1+SCS+CCIE+CCIS_0, &TACCTL0
            ; bits 15-14 Capture mode 0:1 capture on rising edge (CM_1),
            ; 1:0 capture on falling edge (CM_2), 1:1 capture on both edges (CM_3).

            ; bit 11 SCS - synchronize capture source, 0 - async mode, 1 - sync mode
            ; CCISx bits 13-12 these bits select the the TACCRx Input Signal,
            ;   0:0 CCIxA (00h) , 0:1 CCIxB (01h), 1:0 (02h) from GND, 1:1 (03h) from Vcc

			; bit4 - capture/compare interrupt enable
			; mov.w #CAP+CM_3+SCS+CCIE+CCIS_0, &TACCTL0 ; on both edges
			mov.w	#TASSEL_1+ID_3+MC_2+TACLR, &TACTL
			; bit9:8 TASSEL stands for timer A clock source select,
			;  00b = TACLK, 01b = ACLK, 10b = SMCLK, 11 = INCLK/ TBCLK
			; bits 7:6 ID is the input divider, 00b: /1, 01b: /2, 10b: /4, 11b: /8
			; bits5:4 MC mode control, MC_0 stop mode, MC_1 upmode - counts up to TACCR0 value
			; MC_2 conitunous mode - timer counts up to 0FFFFh, MC_3 up/down mode
			clr.w	R6
			clr.w	R7
			bis.w	#GIE+LPM1, SR

; ------------------------------------------------------------------------------
TA0_ISR    ; timer A zero interrupt service routine
;-------------------------------------------------------------------------------
			inc.w	R6
			cmp.w	#1d, R6
			jne		Loop1
			bis.w	#TACLR, &TACTL

Loop1:		cmp.w	#2d, R6
			jne		Loop2
			mov.w	&TACCR0, R7
			clr.w	R6

Loop2:		reti
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
