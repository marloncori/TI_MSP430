#include <msp430.h> 

#define GREEN_LED     BIT0
#define RED_LED       BIT6
#define TOGGLE        (P1OUT = GREEN_LED | RED_LED)
#define BUTTON        BIT3

void stop_watchdog(void);
void setup_leds(void);
void init_button(void);
void low_power_mode(void);
void clear_interrupt_flag(void);

void main(void)
{
    stop_watchdog();
    setup_leds();
    init_button();

    _enable_interrupts();
    low_power_mode();
}

void stop_watchdog(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    // stop wdt
}

void setup_leds(void)
{
    P1DIR  = GREEN_LED | RED_LED;
    P1OUT  = 0x00;
}

void init_button(void)
{
    P1IE   = BUTTON;
    P1IES  = BUTTON;
    clear_interrupt_flag();
}

void clear_interrupt_flag(void)
{
    P1IFG  = 0x00;
}

void low_power_mode(void)
{
    LPM4;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    TOGGLE;

    BCSCTL3 |= LFXT1S_2;
    WDTCTL   = WDT_ARST_1000;
    P1IFG  = 0x00;
}
