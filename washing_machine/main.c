#include <msp430.h> 

/* a washing machine application with five buttons,
 * one step motor with driver, two leds, 7805 voltage regulator */
// FIVE BUTTON PRESS DETECTION
#define ONOFF  ((P2IFG & 0x40) == 0x40)
#define RSPEED ((P2IFG & 0x04) == 0x04)
#define NWASH  ((P2IFG & 0x08) == 0x08)
#define PWASH  ((P2IFG & 0x10) == 0x10)
#define FSPIN   ((P2IFG & 0x20) == 0x20)

#define RED_LED_TOGGLE   (P2OUT ^= 0x01)
#define GREEN_LED_TOGGLE (P2OUT ^= 0x02)

#define NORMAL_WASH 1
#define PREWASH     2
#define FINAL_SPIN  3

int program;
int rotation_speed = 0;
int open = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P2SEL  = 0x00;
	P1DIR  = 0xFF;
	P2DIR  = 0x83;
	P2REN  = 0x7C;
	P1OUT  = 0x00;
	P2OUT  = 0x7C;

	//config interrupt
	P2IE   = 0x7C;
	P2IES  = 0x7C;
	//clear interrupt flag
	P2IFG  = 0x00;
	
	_enable_interrupts();
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if(ONOFF)
    {
        open ^= 1;
        RED_LED_TOGGLE;
        P2IFG &= ~0x40;
    }
    if((RSPEED) && (open == 1))
    {
        rotation_speed ^= 1;
        GREEN_LED_TOGGLE;
        P2IFG &= ~0x04;
    }
    if((NWASH) && (open == 1))
    {
        program = NORMAL_WASH;
        P2IFG &= ~0x08;
    }
    if((PWASH) && (open == 1))
    {
        program = PREWASH;
        P2IFG &= ~0x10;
    }
    if((FSPIN) && (open == 1))
    {
        program = FINAL_SPIN;
        P2IFG &= ~0x20;
    }
    // clear port 2 interrupt flag
    P2IFG = 0x00;
}
