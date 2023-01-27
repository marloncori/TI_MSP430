#include <msp430.h> 

/**
 * The frequency of the ACLK clock can be obtained from the P1.2 and
 * from the P1.6 the frequency of SMCLK can be check.
 *
 * Setting P1DIR and P1SEL. as well as clearing P1SEL2 for the P1.4
 * the SMCLK is configured at P1.4.
 *
 * Setting P1DIR and P1SEL. as well as clearing P1SEL2 for the P1.2
 * the ACLK is configured at P1.0.
 *
 */
void main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
//--------------------------------------------------------------------------------------------
    //  F R E Q U E N C Y
//--------------------------------------------------------------------------------------------
	/* Settings for the DCO - high frequency internal oscillator */
	DCOCTL  = CALDCO_16MHZ;  // calibration data of oscillator
	BCSCTL1 = CALBC1_16MHZ;

	/* other possibilities:
	DCOCTL = CALDCO_12MHZ;
	BCSCTL1 = CALBC1_12MHZ;
        or
	DCOCTL = CALDCO_8MHZ;
	BSCTL1 = CALBC1_8MHZ;
        or
    DCOCTL = CALDCO_1MHZ;
    BSCTL1 = CALBC1_1MHZ; */

//--------------------------------------------------------------------------------------------
	// A C L K
//--------------------------------------------------------------------------------------------
	/* SElection of ACLK */
	BCSCTL3 &= (LFXT1S_3); // restarts the ACLK selection bits
	/*or LFXT1S_0: external crystal, LFXT1S_2: VLO CLK, LFXT1S_3: digital clock*/
	BCSCTL3 |= LFXT1S_2;
	// selects VLOCLK as auxiliary clock (ACLK)

	/* ACLK division */
	BCSCTL1 &= ~DIVA_3; // clear bits
	/* DIVA_0: ACLK/1, DIVA_1: ACLK/2, DIVA_2: ACLK/4, DIVA_3: ACLK/8 */
	BCSCTL1 |= DIVA_0; //divide ACLK by 1

//--------------------------------------------------------------------------------------------
   // M C L K
//--------------------------------------------------------------------------------------------
	/* SElection of MCLK*/
	BCSCTL2 &= SELM_3; // DCO CLK is the high frequency internal oscillator as clock
	/*SELM_0: DCOCLK, SELM_1: DCOCLK, SELM_2: ACLK, SELM_3: ACLK */
	BCSCTL2 |= SELM_0;

	/* MCLK division */
	BCSCTL2 &= DIVM_3;
	/* DIVM_0: MCLK/1, DIVM_1: MCLK/2, DIVM_2: MCLK/4, DIVM_3: MCLK/8 */
	BCSCTL2 |= DIVM_0;

//--------------------------------------------------------------------------------------------
   // S M C L K
//--------------------------------------------------------------------------------------------
    /* SElection of SMCLK*/
    BCSCTL2 &= ~SELS; // DCO CLK is the high frequency internal oscillator as clock
    /* 0: DCOCLK or 1: ACLK (BCSCTL2 |= SELS) */

    /* SMCLK division */
    BCSCTL2 &= DIVS_3;
    /* DIVS_0: SMCLK/1, DIVS_1: SMCLK/2, DIVS_2: SMCLK/4, DIVS_3: SMCLK/8 */
    BCSCTL2 |= DIVS_0;
//--------------------------------------------------------------------------------------------

    // P1.0 = ACLk, P1.4 = SMCLK
    P1DIR |= BIT0 | BIT4;
    P1SEL |= BIT0 | BIT4;
    P1SEL2 = 0x00;

    while(1){}
}
