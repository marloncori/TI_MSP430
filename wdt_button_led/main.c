#include <msp430.h> 

#define GREEN_LED BIT0
#define BUTTON    BIT3

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR  = GREEN_LED;
	P1OUT  = 0x00;
	P1IE   = BUTTON;
	P1IES  = BUTTON;
	P1IFG  = 0x00;
	
	_enable_interrupts();
	LPM4;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    P1OUT = GREEN_LED;

    BCSCTL3 |= LFXT1S_2;
    WDTCTL   = WDT_ARST_1000;
    P1IFG  = 0x00;
}
