#include <msp430.h> 

/*
 * This code by Westlund [36] uses the Analog to Digital Converter
*  ADC10 incorporated in the the MSP430 model. A LED connected to pin 0 in port P1
*  will be lit if the voltage at terminal A1 is greater than 0.5*AVcc, and will be turned
*  off otherwise.
*  Looking at the MSP430 pinout, we find that terminal A1 shares the pin with bit 1
*  of port P1, or P1.1. we should therefore select the ADC.
 * */

void main(void) {

   WDTCTL = WDTPW + WDTHOLD; // Stop WDT

   ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
   ADC10CTL1 = INCH_1; // input A1
   ADC10AE0 |= 0x02; // PA.1 ADC option select

   P1DIR |= 0x01; // P1.0 to output direction

   for(;;){
       ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
       __bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit
      if(ADC10MEM < 0x1FF){
          P1OUT &= ~0x01; // Clear P1.0 LED off
      }
      else {
          P1OUT |= 0x01; // Set P1.0 LED on
      }
   }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
   __bic_SR_register_on_exit(CPUOFF);// Clear CPUOFF bit from 0(SR)
}
