#include <msp430.h> 
#define RedLed    BIT0
#define GreenLed  BIT6
#define Button    BIT3

/**
 * main.c
 */

unsigned int count = 0;

int main(void)
{
   WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR = RedLed | GreenLed;
	P1OUT = RedLed;

	P1IE  = Button;
	P1IES = Button;
	P1IFG = 0x00;

	TACCTL0 = CCIE;  // enable interrupts on compare 0
	TACCR0 = 0xFFFF; // upper limit for counting

	TACTL = TASSEL_2 | ID_3 | MC_3 | TACLR;
	// use clock from SMCLK, divide input by 8,
	// select up down mode and clear timer A

	_enable_interrupts();
	LPM1;
	
	return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){

    count += 1;
    if(count == 5){
        P1OUT ^= GreenLed | RedLed;
        count = 0;
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void){

    P1OUT = RedLed;
    count = 0;
    TACTL |= TACLR;
    P1IFG = 0x00;
}
