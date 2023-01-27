#include <msp430.h> 
#define LED          BIT0
#define LED2         BIT6
#define TOGGLE_LEDS (P1OUT ^= LED2 | LED)

/**
 * usage of TAIV. The red LED toggles when an overflow occurs
 * Capture/compare block is not used.
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR  = LED | LED2;
	P1OUT  = LED;

	TACTL  = TASSEL_2 | ID_3 | MC_3 | TAIE;
	TACCR0 = 62500;
	
	_enable_interrupts();
	LPM1;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A(void)
{
    switch(TAIV)
    {
      case 0x02: break;
      case 0x04: break;
      case 0x0A: TOGGLE_LEDS;
                 break;
    }
}
