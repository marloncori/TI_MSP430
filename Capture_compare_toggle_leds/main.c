#include <msp430.h> 
#define RedLed    BIT0
#define GreenLed  BIT6

/** Toggling the red and green LEDs with various options in C
 *
 * This code is provided to observe the effects of different timer settings.
 * Here, the red and green LEDs toggle. Each disabled setting can be enabled to see its
 * effect. Do not forget to disable the previous active setting when a new one is enabled.
 *
 */

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR = RedLed | GreenLed;
	P1OUT = RedLed;

	TACCTL0 = CCIE;
	//TACTL = TASSEL_2 | ID_3 | MC_2 | TACLR;
	// Use clock from SMCLK, input divider by 8, continuous mode, clear timer A

    //TACTL = TASSEL_2 | ID_0 | MC_2 | TACLR;
    // Use clock from SMCLK, input divider by 1, up mode, clear timer A
	// TACCR0 = 19999; // Upper limit for counting before overflowing
    //TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // divided by 8, up mode

	TACCR0 = 0xFFFF; // max count, 65535
    TACTL = TASSEL_2 | ID_3 | MC_3 | TACLR; // up / down mode

    _enable_interrupts();
    LPM1; // enter low power mode

	return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){

    P1OUT ^= RedLed | GreenLed;
}
