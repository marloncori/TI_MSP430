#include <msp430.h> 
#include "lcd.h"

/*
 * In the second ADC example, given in Listing 11.6, the voltage level at pin A1 is
checked within an infinite loop. This level is converted to the corresponding floating-point
representation. Then it is saved in the variable voltage. This variable can be observed
through the Watch window.
 */
char buffer[15];
char message[] = " Read voltage:";
volatile float voltage;

void Stop_Watchdog_Tmr(void);
void Pin_Config(void);
void ADC_Config(void);
void ADC_Start(void);
void Show_Value(char volts[15]);

void main(void)
{

	Stop_Watchdog_Tmr();

	Pin_Config();
	ADC_Config();

	ADC_Start();
	Lcd_Init();
	
	for(;;)
	{
	  ADC10CTL0 |= ADC10SC;
	  while(ADC10CTL1 & BUSY);

	  // wait if ADC10 core is active
	  voltage = ((ADC10MEM * 3.55) / 0x03FF);
	  FloatToCharPtr(voltage, buffer, 10);
      Show_Value(buffer);
	}

}

void Stop_Watchdog_Tmr(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}

void Pin_Config(void){
    P1DIR = BIT4;
}

void ADC_Config(void){
    ADC10CTL0 = SREF_0 | ADC10SHT_2 | ADC10ON;
    ADC10CTL1 = INCH_1 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;
}


void ADC_Start(void){
    ADC10AE0 = BIT1;
    ADC10CTL0 |= ENC;
}

void Show_Value(char volts[15]){
    Lcd_GoTo(1, 1);
    Lcd_WrtStr(message);

    Lcd_GoTo(2, 4);
    Lcd_WrtStr(volts);
}

