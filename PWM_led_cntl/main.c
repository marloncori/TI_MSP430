#include <msp430.h> 


/**
 * main.c
 */
unsigned char Count, First_Time;
unsigned int REdge1, REdge2, FEdge;

void main(void)
{
  unsigned int Period, ON_Period;
  unsigned char DutyCycle;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

  P1DIR |= BIT0;                            // P1.0/LED Output
  P1OUT &= ~BIT0;                           // LED off

  if(CALBC1_8MHZ==0xFF) {                   // If calibration constant erased, do not load, trap CPU!
    while(1);
  }

  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_8MHZ;                    // Set DCO to 8MHz
  DCOCTL = CALDCO_8MHZ;

  P1DIR &= ~BIT2;                           // P1.1/TA0.1 Input Capture
  P1SEL |= BIT2;                            // TA0.1 option select

  /* Configure the TA0CCR1 to do input capture
   * CCIE - capture compare interrupt enable,
   * bit13-12 CCIS - capture/compare iput select, it selects the TACCRx input signal
   *    00 = CCIxA,   01 = CCIxB,   10 = GND,   11 = Vcc
   * SCS - synchronize capture source -> 0: async, 1: sync mode,
   * SCCI - sychronized capture/compare input,
   *    it is latched with EQUx signal and can be read via this bit (bit10 of TACCTLx)
   * bit8 CAP - 0: compare mode, 1: capture mode
   * bit7-5 OUTMOD - output mode
   *   - 000 = out bit value  010 = toggle/reset   100 = toggle    110 = toggle/set
   *   - 001 = set            011 = set/reset      101 = reset     111 = reset/set
   **/
  TA0CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0; // capture mode options -> CM_0: no cap,
                                             // CM_1: o rising edge, CM_2: falling edge
                                            // TA0CCR1 Capture mode; CCI1A; Both
                                            // CM_3: Rising/Falling Edge; interrupt enable
  TA0CTL |= TASSEL_2 + MC_2 + TACLR;
  /* SMCLK as source, Continuous Mode; start timer
   * TASSEL_0: TACLK, TASSEL_1: ACLK, TASSEL_2: CMCLK, TASSEL_3: INCLK / TBCLK
   *
   * input divider -> ID_0: ID_1: ID_2: ID_3:
   *
   * mode control -> MC_0: stop mode, MC_1: up mode (counts up to TACCRx set value,
   *                 MC_2: continuous mode, counts up to 0FFFFh,
   *                 MC_3: up/down mode, counts up to TACCRx set value and down to zero.
   * */

  // Variable Initialization
  Count = 0x0;
  First_Time = 0x01;

  while(1) {

      __bis_SR_register(LPM0_bits + GIE);    //the same as _enable_interrupts(); LPM0;
      __no_operation();                     // For debugger

      // On exiting LPM0
      if (TA0CCTL1 & COV)                   // Check for Capture Overflow
          while(1);                         // Loop Forever

      Period = REdge2 - REdge1;             // Calculate Period
      ON_Period = FEdge - REdge1;             // On period
      DutyCycle = ((unsigned long)ON_Period*100/Period);
      if(DutyCycle> 40) {
          P1OUT |= BIT0;
      } else {
          P1OUT &= ~BIT0;
      }
  }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void)
{
  switch(__even_in_range(TA0IV,0x0A))
  {
      case  TA0IV_NONE: break;              // Vector  0:  No interrupt
      case  TA0IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
        if(TA0CCTL1 & CCI) {                // Capture Input Pin Status
            // Rising Edge was captured
            if(!Count) {
                REdge1 = TA0CCR1;
                Count++;
            } else {
                REdge2 = TA0CCR1;
                Count=0x0;
                __bic_SR_register_on_exit(LPM0_bits + GIE);
                // Exit LPM0 on return to main
            }

            if(First_Time) {
                First_Time = 0x0;
            }
        } else {
            // Falling Edge was captured
            if(!First_Time) {
                FEdge = TA0CCR1;
            }
        }
        break;
      case TA0IV_TACCR2: break;             // Vector  4:  TACCR2 CCIFG
      case TA0IV_6: break;                  // Vector  6:  Reserved CCIFG
      case TA0IV_8: break;                  // Vector  8:  Reserved CCIFG
      case TA0IV_TAIFG: break;              // Vector 10:  TAIFG
      default:  break;
  }
}
