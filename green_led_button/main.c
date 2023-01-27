#include <msp430.h> 

#define LED BIT6
#define BTN BIT3

/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR = LED;
	P1REN = BTN;
	P1OUT = BTN;

	do{
	 if((P1IN & BTN) == 0x00)
	 {
	     P1OUT |= LED;
	 }
	 else
	 {
	     P1OUT &= ~LED;
	 }
	}while(1);
}
