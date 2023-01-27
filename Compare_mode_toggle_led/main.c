#include <msp430.h> 
#define  Led_Toggle() (P1OUT ^= BIT0)

/*  TOGGLE LED USING THE TIMER INTERRUPT IN COMPARE MODE
 * Here the ISR toggles the red LED based on the compare mode configuration.
 *
 */
void Stop_Watchdog_Timr(void);
void Led_Init(void);
void TimerA_UpperLimit_Count(void);
void EnableInterrupts_Compare0(void);
void TimerA_Setup_Start(void);
void Enter_Low_Pwr_Mode(void);
//----------------------------------------------------------------------------------------------
int main(void)
{
    Stop_Watchdog_Timr();

    Led_Init();
    TimerA_UpperLimit_Count();

    EnableInterrupts_Compare0();
    TimerA_Setup_Start();

    _enable_interrupts();
    Enter_Low_Pwr_Mode();

    return 0;
}
//-----------------------------------------------------------------------------------------------
void Stop_Watchdog_Timr(void){
    WDTCTL = WDTPW | WDTHOLD;
}
//-----------------------------------------------------------------------------------------------
void Led_Init(void){
    P1DIR = BIT0;
    P1OUT = BIT0;
}
//-----------------------------------------------------------------------------------------------
void TimerA_UpperLimit_Count(void){
    TACCR0 = 49999;
}
//-----------------------------------------------------------------------------------------------

void EnableInterrupts_Compare0(void){
    TACCTL0 = CCIE;
}
//-----------------------------------------------------------------------------------------------

void TimerA_Setup_Start(void){
    /* TASSEL_0 = TACLK, TASSEL_1 = ACLK
      ;TASSEL_2 = SMCLK,
      ; modes for the divider for the input clock:
      ; ID_0 = /1, ID_1 = /2, ID_2 = /4, ID_3 = /8,
      ; mode control options: MC_1 up mode, counts up to TACCR0 set value
      ; MC_2: continuous mode, counts up to 0FFFFh
      ; MC_3: up/down mode - counts till TACCR0 set value and then down to zero.
      ; TAIE - timer A interrupt enable in the time A control register */
    TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;
}
//-----------------------------------------------------------------------------------------------
void Enter_Low_Pwr_Mode(void){
    LPM1;
}
//-----------------------------------------------------------------------------------------------
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    Led_Toggle();
}
//-----------------------------------------------------------------------------------------------
