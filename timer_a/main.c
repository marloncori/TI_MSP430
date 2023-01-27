#include <msp430.h> 

//40 sec. MAX, connect P1.1 to P1.3
int count  = 0;
int result = 0;
float sec  = 0;

void main(void)
{
	WDTCTL   = WDTPW | WDTHOLD;	// stop watchdog timer
	BCSCTL3 |= LFXT1S_2;
	P1SEL    = 0x02;
	
	// TACCTL0 = CAP | CM_3 | SCS | CCIE | CCIS_0;
	// capture at rising and falling edges
	TACCTL0 = CAP | CM_2 | SCS | CCIE | CCIS_0;
	// capture at falling edges
	TACTL   = TASSEL_1 | ID_3 | MC_2 | TACLR;
	_enable_interrupts();
	LPM1;

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    count++;
    // clear TAR at first press
    if(count == 1)
    {
       TACTL |= TACLR;
    }
    //get the final value at second press
    if(count == 2)
    {
       result = TACCR0;
          sec = (float)result/1500;
        count = 0;
    }

}
