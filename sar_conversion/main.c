#include <msp430.h> 

#define BIT_SIZE 10

float Vref = 3.6;
float Vin  = 3.3;
float thresh;
float quantized = 0;

int count;
int bit_val;
int bits[BIT_SIZE];

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Vref /= 2;
	
	for(count = 0; count < BIT_SIZE; count++){
	  if(Vin >= thresh)
	  {
	      bit_val = 1;
	      thresh += Vref;
	  }
	  else
	  {
	      bit_val = 0;
	      thresh -= Vref;
	  }
	  bits[count] = bit_val;
	  quantized += 2 * Vref * bit_val;
	}

	while(1){}
}
