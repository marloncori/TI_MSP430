#include <msp430.h> 

#define LED                  BIT0
#define LED2                 BIT6
#define TOGGLE_LEDS          (P1OUT ^= LED | LED2)
#define CLEAR_INTERRUPT_FLAG (P1IFG  = 0x00)
#define BUTTON               BIT3

unsigned int count = 0;
unsigned int greenCount = 0;
unsigned int redCount = 0;
unsigned int state = 0;
unsigned int done = 0;

void setup_leds();
void init_button();
void enable_interrupt_compare0();
void config_timer();
void clear_leds();

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	setup_leds();

	init_button();
	enable_interrupt_compare0();

	config_timer();
	_enable_interrupts();
	
	do{
	  if(done > 5)
	  {
	      clear_leds();
	      _disable_interrupts();
	      LPM4;
	  }
	  else
	  {
	      LPM1;
	  }
	} while(1);

}

void setup_leds(void)
{
    P1DIR  = LED | LED2; // output
    P1OUT  = LED; //LED high, LED2 high
}

void init_button(void)
{
    P1IE  = BUTTON;
    P1IES = BUTTON;
    P1IFG = 0x00;
}

void enable_interrupt_compare0(void)
{
    TACCTL0  = CCIE;
}

void config_timer(void)
{
   TACCR0  = 0xFFFF; // Upper limit for count
   TACTL   = TASSEL_2 | ID_3 | MC_3 | TACLR;
   // use clock form SMCLK, divide clock by 8
   // use continuous up down mode, clear Timer A
}

void clear_leds(void)
{
    P1OUT = 0x00;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
    LPM1_EXIT;
    count++;
    if(count == 30)
    {
        TOGGLE_LEDS;
        state = !state;
        count = 0;
        done++;
    }

}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    if(state == 0)
    {
        redCount++;
    }
    else
    {
        greenCount++;
        CLEAR_INTERRUPT_FLAG;
    }
}
