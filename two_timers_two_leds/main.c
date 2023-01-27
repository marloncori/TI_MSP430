#include <msp430.h> 

#define LED          BIT0
#define LED2         BIT6
#define TOGGLE_LED  (P1OUT ^= LED)
#define TOGGLE_LED2 (P1OUT ^= LED2)

void setup_leds();
void init_timers();
void run_timers();
void stop_watchdog_timer();
void interrupt_enable_compare0();


void main(void)
{
    stop_watchdog_timer();

    setup_leds();
    init_timers();

    interrupt_enable_compare0();
    run_timers();

    _enable_interrupts();
    LPM1;
}

void stop_watchdog_timer(void){
    WDTCTL = WDTPW | WDTHOLD;
}

void setup_leds(void)
{
    P1DIR  = LED | LED2; // output
    P1OUT  = LED | LED2; //LED high, LED2 high
}

void init_timers(void)
{
    BCSCTL3 |= LFXT1S_2;
}

void interrupt_enable_compare0(void)
{
    TACCTL0  = CCIE;
    TA1CCTL0 = CCIE;
}

void run_timers(void)
{
    TACCR0  = 62500; // Upper limit for count
    TACTL   = TASSEL_2 | ID_3 | MC_2 | TACLR;
    TA1CCR0 = 6000;
    TA1CTL  = TASSEL_1 | ID_3 | MC_3 | TACLR;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
    TOGGLE_LED;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void)
{
    TOGGLE_LED2;
}
