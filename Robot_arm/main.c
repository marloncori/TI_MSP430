/* drselim MSP430 & Robot ARM
  Please don't forget to give credits while using the code
 video link for the description of the code:
 https://youtu.be/b7LaDw4Rz74 */

#include <msp430.h> 
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <math.h>

unsigned int adc[8];

int centerpwm;
int leftpwm;
int rightpwm;
int clamppwm;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    ADC10CTL1 = INCH_7 + ADC10DIV_0 + CONSEQ_3 + SHS_0;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + MSC + ADC10ON;
    ADC10AE0 = BIT7 + BIT5 + BIT4 + BIT0;
    ADC10DTC1 = 8;


    while(1){
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & BUSY);
        ADC10CTL0 |= ENC + ADC10SC;
        ADC10SA = (unsigned int)adc;
        centerpwm = floor((adc[0]*1.95)+350);
        leftpwm = floor((adc[2]*1.95)+350);
        rightpwm = floor((adc[3]*1.95)+350);
        clamppwm = floor((adc[7]*1.95)+350);
        if (P1IN & 0x08){

            P1DIR  =  0x04; // Set P1.2  to output-direction
            P2DIR  =  0x02; // Set P2.1  to output-direction
            P1SEL  |=  0x04; // Set P1.2
            P2SEL  |=  0x02; // Set P2.1
            TA1CCR0  = 20000;             // Period
            TA0CCR0  = 20000;             // Period
            TA0CCR1  = centerpwm+1;              // Duty-cycle
            TA1CCR1 = clamppwm+1;
            TA1CTL   = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode
            TA0CTL   = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode
            TA1CCTL1 = OUTMOD_7;         // Reset - set
            TA0CCTL1 = OUTMOD_7;         // Reset - set
        }
        else {
            P1DIR  =  0x40; // Set P1.6 to output-direction
            P2DIR  =  0x04; // Set P2.2 to output-direction
            P1SEL  |=  0x40; // Set P1.6
            P2SEL  |=  0x04; // Set P2.2
            TA1CCR0  = 20000;             // Period
            TA0CCR0  = 20000;             // Period
            TA0CCR1  = leftpwm+1;              // Duty-cycle
            TA1CCR1 = rightpwm+1;
            TA1CTL   = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode
            TA0CTL   = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode
            TA1CCTL1 = OUTMOD_7;         // Reset - set
            TA0CCTL1 = OUTMOD_7;         // Reset - set
        }
        __delay_cycles(100);
    }
}
