
#include <msp430.h> 

#define motor_start() (P1SEL |= 0x04)
#define motor_stop()  (P1SEL &= ~0x04)

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
             motor_start();
             TACTL = MC_1 | ID_2 | TASSEL_2 | TACLR;
             control = 1;
          }
       }
    }
}

void pin_config(void)
{
   P1DIR = 0xFE;
   P1OUT = 0x10;
}

void timer_config(void)
{
   WDTCTL   = WDTPW | WDTHOLD;
   BCSCTL3 |= LFXT1S_2;

   TACCTL0 = CCIE;
   TACCTL1 = OUTMOD_7;
   TACTL   = MC_0;
   TACCR0  = 49;
   TACCR1  = 25;
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
  if(count == 20000)
  {
     motor_stop();
     TACTL = MC_0;
     control = 0;
     count = 0;
  }
}
