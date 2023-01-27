#include <msp430.h> 

/*
 * An external parallel ADC is connected to port P1, and handshaking
* is done with port P2, bits P2.0 and P2.1. The ADC starts data conversion when P2.0
* makes a low-to-high transition. New datum is ready when P2.1 is high. The objective
* of the code is to read and store 10 data in memory. Normal programs would do
* something else after data is collected. For illustration purposes, let the “something
* else” be sending a pulse to P2.3. After set up, the main will call in an infinite loop
* the two functions for reading and for sending the pulse.
 * */

unsigned int dataRead[10]; // Data in memory

void StoreData(void);
void ProcessData(void);

void main(void) {

   WDTCTL = WDTPW | WDTHOLD; //Stop watchdog timer
   P2DIR  = BIT0 | BIT3; // Set output directions for pins

  do{
    P2OUT = 0; // Initialize with no outputs
    StoreData(); // Call for data storage
    ProcessData(); // Call for other processing
  }while(1);

}

void StoreData (void){
  volatile unsigned int i; // not affected by optimization

  for(i=10; i>0; i--){
      P2OUT |= BIT0; // Request conversion
      while (P2IN & 0x08!=0x08); // wait for conversion
      dataRead[i] = P2IN;
      P2OUT &= ~BIT0; // prepare for new conversion.
  }
}

void ProcessData(void){
   volatile unsigned int i; // not affected by optimization

   P2OUT |= BIT3; // Set voltage at P2.3
   for(i=65000; i>0; i--); // pulse width
    P2OUT &= ~BIT3; // Complete pulse
}
