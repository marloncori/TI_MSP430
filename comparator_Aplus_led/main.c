#include <msp430.h> 

#define RED_LED   BIT0
#define GREEN_LED BIT6

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR  = RED_LED | GREEN_LED;

	CACTL1 = CARSEL+CAREF_1+CAON;
	// 0.25 Vcc = -comp, on
	CACTL2 = P2CA4;
	// P1.1/CA1 = +comp
	
	while(1)
	{
	    if(CAOUT & CACTL2)
	    {
	        P1OUT = GREEN_LED;
	    }
	    else
	    {
	        P1OUT = RED_LED;
	    }
	}
}
