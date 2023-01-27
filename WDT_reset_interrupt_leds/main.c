#include <msp430.h> 
#define RUNNING  1

/**********************************************************************************************/
 // GLOBAL VARIABLES
/**********************************************************************************************/
volatile unsigned char counter = 10;

/**********************************************************************************************/
// FUNCTION PROTOTYPES
/**********************************************************************************************/
void StopWatchdogTimer(void);
void NonMaskableInterruptCfg(void);
void ResetRoutineCfg(void);
void InternalClockCfg(void);
void GPIO_Config(void)
;
/**********************************************************************************************/
 // START OF PROGRAM
/**********************************************************************************************/
void main(void)
{
    StopWatchdogTimer();

	InternalClockCfg();
	GPIO_Config();
	
    NonMaskableInterruptCfg();

    do {
       if(!counter){
         ResetRoutineCfg();
       }
    } while(RUNNING);
}

/**********************************************************************************************/
 // INTERRUPT SERVICE ROUTINE
/**********************************************************************************************/
#pragma vector = NMI_VECTOR
__interrupt void NMI_ISR(void){

   unsigned long i = 0;
   if((IFG1 & NMIIFG) == NMIIFG){
      P1OUT &= ~BIT6;
      P1OUT ^= BIT0;
      counter--;
      for(i=1000000;i>0;i--){}
       IFG1 &= NMIIFG;
       IE1 |= NMIIE;
   }
}

/**********************************************************************************************/
 // FUNCTION DEFINITIONS
/**********************************************************************************************/
void StopWatchdogTimer(void){
   WDTCTL = WDTPW | WDTHOLD; // para el can de guardia;
}
/**********************************************************************************************/
void NonMaskableInterruptCfg(void){
    /* WDTPW = 0x05A, llave para modificar el registro
     * WDTNMIES 1: borde de baja, 0: borde de subida en NMI reset pin
     * WDTNMI 1: NMI function, 0: reset function for button
     */
    WDTCTL = WDTPW | WDTNMIES | WDTNMI | WDTHOLD;
    IFG1  &= ~NMIIFG;// limpia la bandera por si acaso
    IE1   |= NMIIE; // habilita la interruption local de pin NMI

}
/**********************************************************************************************/
void ResetRoutineCfg(void){
    IFG1 &= ~NMIIFG;
    IE1  &= ~NMIIE; // desabilita interrupcion
    WDTCTL = WDTPW | WDTHOLD; // para el can de guardia;
}
/**********************************************************************************************/
void InternalClockCfg(void){
        /* Settings for the DCO - high frequency internal oscillator */
        DCOCTL  = CALDCO_16MHZ;  // calibration data of oscillator
        BCSCTL1 = CALBC1_16MHZ; // aprox. 15.62 to 15.72 MHz

        /* other possibilities:
        DCOCTL = CALDCO_12MHZ;   DCOCTL = CALDCO_8MHZ;  DCOCTL = CALDCO_1MHZ;
        BCSCTL1 = CALBC1_12MHZ;  BSCTL1 = CALBC1_8MHZ;  BSCTL1 = CALBC1_1MHZ; */

        /* SElection of ACLK */
        BCSCTL3 &= (LFXT1S_3); // restarts the ACLK selection bits
        /*or LFXT1S_0: external crystal, LFXT1S_2: VLO CLK, LFXT1S_3: digital clock*/
        BCSCTL3 |= LFXT1S_2;  // aprox. 14.43 to 14.63 MHz
        // selects VLOCLK as auxiliary clock (ACLK)

        /* ACLK division */
        BCSCTL1 &= ~DIVA_3; // clear bits
        /* DIVA_0: ACLK/1, DIVA_1: ACLK/2, DIVA_2: ACLK/4, DIVA_3: ACLK/8 */
        BCSCTL1 |= DIVA_0; //divide ACLK by 1

        /* SElection of MCLK*/
        BCSCTL2 &= SELM_3; // DCO CLK is the high frequency internal oscillator as clock
        /*SELM_0: DCOCLK, SELM_1: DCOCLK, SELM_2: ACLK, SELM_3: ACLK */
        BCSCTL2 |= SELM_0;

        /* MCLK division */
        BCSCTL2 &= DIVM_3;
        /* DIVM_0: MCLK/1, DIVM_1: MCLK/2, DIVM_2: MCLK/4, DIVM_3: MCLK/8 */
        BCSCTL2 |= DIVM_0;

        /* SElection of SMCLK*/
        BCSCTL2 &= ~SELS; // DCO CLK is the high frequency internal oscillator as clock
        /* 0: DCOCLK or 1: ACLK (BCSCTL2 |= SELS) */

        /* SMCLK division */
        BCSCTL2 &= DIVS_3;
        /* DIVS_0: SMCLK/1, DIVS_1: SMCLK/2, DIVS_2: SMCLK/4, DIVS_3: SMCLK/8 */
        BCSCTL2 |= DIVS_0;
}
/**********************************************************************************************/
void GPIO_Config(void){
    P1DIR = 0xFF;
    P1OUT = 0xFF;
    P1REN = 0x00;
}
/**********************************************************************************************/
 //  END OF APPLICATION
/**********************************************************************************************/
