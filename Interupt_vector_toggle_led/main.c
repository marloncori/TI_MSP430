#include <msp430.h> 
//-----------------------------------------------------------------------------------------------
#define  RED_LED       BIT0
#define  Led_Toggle() (P1OUT ^= RED_LED)
//-----------------------------------------------------------------------------------------------
/**
 * TAIV - timer A interrupt vector
 * In this example Here the red LED toggles when an overflow occurs. The timer A counts
 * up 62500, not to its max value of 65535, then it counts down to 0. The whole counting is 12500
 * The overflow happens roughly every second (based on the parameter settings).
 * Here, the capture/compare block is not used.
 */
//-----------------------------------------------------------------------------------------------
void Stop_Watchdog_Timr(void);
void Led_Init(void);
void TimerA_Config(void);
void Enter_Low_Pwr_Mode(void);
//----------------------------------------------------------------------------------------------
int main(void)
{
	Stop_Watchdog_Timr();
	
	Led_Init();
	TimerA_Config();

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
    P1DIR = RED_LED;
    P1OUT = RED_LED;
}
//-----------------------------------------------------------------------------------------------
void TimerA_Config(void){
    /* TASSEL_0 = TACLK, TASSEL_1 = ACLK
      ;TASSEL_2 = SMCLK,
      ; modes for the divider for the input clock:
      ; ID_0 = /1, ID_1 = /2, ID_2 = /4, ID_3 = /8,
      ; mode control options: MC_1 up mode, counts up to TACCR0 set value
      ; MC_2: continuous mode, counts up to 0FFFFh
      ; MC_3: up/down mode - counts till TACCR0 set value and then down to zero.
      ; TAIE - timer A interrupt enable in the time A control register */
    TACTL = TASSEL_2 | ID_3 | MC_3 | TAIE;
    TACCR0 = 62500;
}
//-----------------------------------------------------------------------------------------------
void Enter_Low_Pwr_Mode(void){
    LPM1;
}
//-----------------------------------------------------------------------------------------------
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A(void){

    switch(TAIV){
       case 0x02: break;
       case 0x04: break;
       case 0x0A: Led_Toggle();
            break;
    }
}
//-----------------------------------------------------------------------------------------------
