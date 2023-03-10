#include <msp430.h> 

/** This is the usage of two timers together in C language.
* Here, the red and green LEDs toggle by interrupts generated by TA0 and TA1 separately.
* We provide the assembly code doing the same operation in a separate file.
* Again, the two timers are used together in this code block.
 *
 */

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	BCSCTL3 = LFXT1S_2 // ; 12 KHz as ACLK source;
	P1DIR = BIT0 | BIT6;
	P1OUT = BIT0 | BIT6;

	TACCR0  = 62500;   // set both timer max count value
	TA1CCR0 = 6000;

	TACCTL0  = CCCIE;  // enable both timer interrupts
	TA1CCTL0 = CCIE;


    /* ; bit9:8 TASSEL stands for timer A clock source select,
    ;  00b = TACLK, 01b = ACLK, 10b = SMCLK, 11 = INCLK/ TBCLK
    ; bits 7:6 ID is the input divider, 00b: /1, 01b: /2, 10b: /4, 11b: /8
    ; bits5:4 MC mode control, MC_0 stop mode, MC_1 up mode - counts up to TACCR0 value
    ; MC_2 continuous mode - timer counts up to 0FFFFh, MC_3 up/down mode */

    // up mode, SMCLK
	TACTL  = CM_1 | ID_3 | TASSEL_2 | TACLR;

 	// up down mode, ACLK
	TA1CTL = CM_3 | ID_3 | TASSEL_1 | TACLR;

	_enable_interrupts();
	LPM1;

	return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    P1OUT ^= BIT0;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void){
    P1OUT ^= BIT6;

}
