#include <msp430.h> 

#define LED_ON()  (P1OUT |= 0x08)
#define LED_OFF() (P1OUT &= ~0x08)

int count = 0;
int control = 0;

void pin_config(void);
void timer_config(void);
void ADC_config(void);

void main(void)
{
	pin_config();
	timer_config();
	ADC_config();
	_enable_interrupts();
	
	for(;;)
	{
	   ADC10CTL0 |= ADC10SC;
	   while((ADC10CTL1 & ADC10BUSY) == ADC10BUSY);

	   if(control == 0)
	   {
	      if(ADC10MEM < 0x0300)
	      {
	         LED_ON();
	         TACTL = MC_1 | ID_3 | TASSEL_1 | TACLR;
	         control = 1;
	      }
	   }
	}
}

void pin_config(void)
{
  P1DIR = 0xFE;
  P1OUT = 0x00;
}

void timer_config(void)
{
  WDTCTL   = WDTPW | WDTHOLD;
  BCSCTL3 |= LFXT1S_2;

  TACCTL0 = CCIE;
  TACTL = MC_0;
  TACCR0 = 1499;
}

void ADC_config(void)
{
  ADC10CTL0  = SREF_0 | ADC10SHT_3 | ADC10ON;
  ADC10CTL1  = INCH_0 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;
  ADC10AE0   = BIT0;
  ADC10CTL0 |= ENC;
}

#pragma vector = TIMER0_A0_VECTOR;
__interrupt void isr_name(void)
{
  count++;
  if(count == 4)
  {
     LED_OFF();
     TACTL = MC_0;
     control = 0;
     count = 0;
  }
}
