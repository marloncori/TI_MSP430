
//------------------------------------------------------------------------------------------------
#include <msp430.h> 
#define RUNNING   1

/**
 * Use Timer_A CCRx units (x = 0, 1, 2), and overflow to generate
* four independent timing intervals. TACCR0, TACCR1 and TACCR2 output units are
* selected with port pins P1.1, P1.2 and P1.3 in toggle mode. The pins toggles when
* the TACCRx register matches the TAR counter. Interrupts are also enabled with all
* TACCRx units, software loads offset to next interval only—as long as the interval
* offset is added to TACCRx, toggle rate is generated in hardware. Timer_A overflow
* ISR is used to toggle P1.0 with software. Proper use of the TAIV interrupt vector
* generator is demonstrated. An external 32KHz watch crystal is used.
*
* AC LK = T AC LK = 32 kHz, MCLK = SMCLK = default DCO 1.2 MHz
* As coded and with T AC L K = 32,768 Hz, toggle rates are:
* -> P1.1 = TACCR0 = 32,768/(2 * 4)   = 4,096 Hz -> 0.000244140625 sec = 244.140625 us
* -> P1.2 = TACCR1 = 32,768/(2 * 16)  = 1,024 Hz -> 0.0009765625 sec = 976.5625 us
* -> P1.3 = TACCR2 = 32,768/(2 * 100) = 163.84 Hz -> 0.006103515625 sec = 6.1035 ms
* --------> P1.0 = overflow = 32,768/(2 * 65,536) = 0.25 Hz -> 4 sec
 *                                        0FFFFh
 */
//------------------------------------------------------------------------------------------------

void Stop_Watchdog_Timer(void);
void Led_Setup(void);
void Timers_Init(void);
void Clk_Config(void);
//------------------------------------------------------------------------------------------------

void main(void)
{
	Stop_Watchdog_Timer();
	Led_Setup();
	Timers_Init();
	Clk_Config();

	_enable_interrupts();
	LPM1;

	while(RUNNING){}
}
//------------------------------------------------------------------------------------------------

void Stop_Watchdog_Timer(void){
    WDTCTL = WDTPW | WDTHOLD;
}

void Led_Setup(void){
    P1DIR |= 0x0E;
    P1OUT |= 0x0F;
}

void Timers_Init(void){
    TACCTL0 = OUTMOD_4 + CCIE;
    TACCTL1 = OUTMOD_4 + CCIE;  // capture compare interrupt enabled
    TACCTL0 = OUTMOD_4 + CCIE; // output mode four is Toggle mode
}

void Clk_Config(void){
    TACTL = TASSEL_1 + MC_2 + TAIE;
    /* use clock from the Auxiliary Clock,
     * continuous mode (counts up to 0xFFFF)
     * timer A interrupt enabled */
}
//------------------------------------------------------------------------------------------------

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    // P1.1 = TACCR0 = 32,768/(2 * 4) = 4,096 Hz -> 0.244140625 ms = 244.140625 us
    TACCR0 += 4; // add offset
}

// Timer A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TA1_ISR(void){
    switch(TAIV){
      // P1.2 = TACCR1 = 32,768/(2 * 16)  = 1,024 Hz -> 0.9765 ms = 976.56 us
      case 2:  TACCR1 += 16;
               break;
      // P1.3 = TACCR2 = 32,768/(2 * 100) = 163.84 Hz -> = 6.1035 ms
      case 4:  TACCR2 += 100;
               break;
      case 10: P1OUT ^= 0x01;  // Timer A3 overflow
               break;
    }
}

//------------------------------------------------------------------------------------------------
