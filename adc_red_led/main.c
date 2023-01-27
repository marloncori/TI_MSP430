#include <msp430.h> 

#define LED BIT0

/*
 * Our first ADC example in the C language is given in Listing 11.5. Here, the ADC10
module is basically used as a comparator. The input voltage level at pin A1 (pin P1.1) is
checked within an infinite loop. If the value there is above a certain level, the red LED is
turned on.
 * */

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR = LED;
	P1OUT = 0x00;

	ADC10CTL0 = SREF_0 | ADC10SHT_2 | ADC10ON;
	// Vcc and Vss references, sample for 16 cycles
	// ADC on

	ADC10CTL1 = INCH_1 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;
	// input channel 1 (A1), trigger using ADC10SC bit,
	// use internal ADC clock, single channel and single conversion

	ADC10AE0 = BIT1;
	// enable conversion
	ADC10CTL0 |= ENC;

	for(;;)
	{
	  // trigger new conversion
	  ADC10CTL0 |= ADC10SC;

	  while(ADC10CTL1 & BUSY);
	  // wait if ADC10 core is active

	  if(ADC10MEM >= 0x0200)
	  {
	     P1OUT = LED;
	  }
	  else
	  {
	     P1OUT = 0x00;
	  }
	}
}
