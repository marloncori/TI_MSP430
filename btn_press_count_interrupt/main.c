#include <msp430.h> 

#define BUTTON  BIT3

unsigned int counter = 0;

void stop_watchdog();
void btn_interrupt_cfg();
inline void clear_interrupt_flag();

void main(void)
{
    stop_watchdog();
    btn_interrupt_cfg();

    _enable_interrupts();
    CPUOFF
    while(1); // wait for an interrupt
}

#pragma vector=PORT1_VECTOR
// define interrupt vector
__interrupt void PORT1_ISR(void)
{
    // Interrupt Service Routine
    counter += 1;
    clear_interrupt_flag();
}
void stop_watchdog(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    // stop watchdog timer
}

void btn_interrupt_cfg(void)
{
    P1IE   = BUTTON; // enable interrupt from port 1
    P1IES  = BUTTON; // interrupt edge select from high to low
    clear_interrupt_flag();
}

inline void clear_interrupt_flag(void)
{
    P1IFG = 0x00;
}
