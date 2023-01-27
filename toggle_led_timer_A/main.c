#include <msp430.h> 

#define LED          BIT0
#define LED2         BIT6
#define TOGGLE_LEDS (P1OUT ^= LED | LED2)
/*
 * ISR toggles the LED based on the compare mode config.
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    P1DIR  = LED | LED2;
    P1OUT  = LED2;


    TACCR0 = 49999; // Upper limit of count for TAR
    TACCTL0 = CCIE; // enable interrupts on compare 0

    TACTL  = TASSEL_2 | ID_3 | MC_1 | TACLR;
    // setup and start timer A

    _enable_interrupts();
    LPM1;

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
    TOGGLE_LEDS;
}
