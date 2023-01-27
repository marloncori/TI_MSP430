#include <msp430.h> 

/**
 * ;MSP430F20xx Demo - USICNT Used as a One-Shot Timer Function, DCO SMCLK
   ;By M. Buccini and L. Westlund - 09/2005
   ;Copyright (c) Texas Instruments, Inc.
   ;---------------------------------------------------------------------

 *  This example introduces yet another of the MSP430 peripherals, the
   universal serial interface or USI [37]. The USI interrupt service routine
   toggles the LED connected to pin 0 in port P1.

     Since the auxiliary clock ACLK is chosen and divided by 128 and then
   the USI counter USICNT is loaded with 32, the USI interrupt flag USIIFG is
   set after a total of 4096 or 128*32 pulses.

    USICNT is reloaded after toggling the LED inside the interrupt service routine.
 */

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer

	BCSCTL3 |= LFXT1S_2;       // ACLK = VLO
	 P1DIR  |= 0x01;           // set P1.0 as output

	USICTL0 |= USIMST;         // Master mode
	USICTL1 |= USIIE;          // counter interrupt, flag remains set
	
 	USICKTCL = USIDIV_7+USISSEL_1; // /128 ACLK
 	USICTL0 &= ~USISWRST;        // USI released for operation

 	__bis_SR_register(LPM3_bits+GIE); // enter LPM3 w/  interrupt

}

/* USI interrupt service routine */
#pragma vector = USI_VECTOR
__interrupt void Universal_Serial_Interface_ISR(void)
{
    P1OUT   ^=  0x01;   // toggle P1.0 with xor
    USICNT   =  0x1F;   // re-load counter
}
