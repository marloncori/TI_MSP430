#include <msp430.h> 

#define SAMPLE_NUMBER 20
/*
 * In the third ADC example, given in Listing 11.7, the internal temperature sensor of
the ADC10 module is used. The temperature is measured 20 times using the DTC module.
The average temperature value is calculated. Again, the average temperature is converted
to the true (scaled) value. Then it is saved in the avg_temp variable. This can also be
observed in the Watch window.
 */

float avg_temp = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	int count;
	unsigned int temperatures[SAMPLE_NUMBER];

	ADC10CTL1 = CONSEQ_2 + INCH_10 + ADC10DIV_7;
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE + MSC;
	// repeat single channel, temperature sensor, ADC10CLK / 8

	ADC10DTC1 = SAMPLE_NUMBER;
	// number of conversions;

	while(ADC10CTL1 & BUSY);
	// wait if ADC10 core is active

	ADC10SA = (unsigned int) temperatures;
	// data buffer start address
	
	ADC10CTL0 |= ENC + ADC10SC;
	// sampling and conversion start

	_enable_interrupts();

	LPM0; // ADC10_ISR will force exit

	for(count = 0; count < SAMPLE_NUMBER; count++)
	{
	  avg_temp += temperatures[count];

	  avg_temp = avg_temp / SAMPLE_NUMBER;
	  avg_temp = ((avg_temp - 673) * 423) / 1024;
	  while(1);
	}
}

#pragma vector = ADC10_VECTOR;
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;
}
