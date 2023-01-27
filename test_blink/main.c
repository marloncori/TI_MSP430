/* --- MILLENIUM ROBOTICS --
 * source: https://embedded-lab.com/blog/introducing-ti-msp430-microcontrollers/
 * LED connected to P1.6 blinks continuously while P1.0 LED blinks
 * only when the user button is pressed, slowing down P1.6 LED’s blink rate.
 * User button is connected to P1.3
 * Version: 15-06-2022 */
#include <msp430.h> 

#define GREEN_LED_STATE  BIT0
#define RED_LED_STATE    BIT6
#define PUSH_BTN_STATE   BIT3

void GPIO_graceInit(void);
void BCSplus_graceInit(void);
void WDTplus_graceInit(void);
void System_graceInit(void);

void main(void)
{
    /* Stop watchdog timer from timing out during initial start-up. */
	WDTCTL = WDTPW | WDTHOLD;
	/* initialize Config for the MSP430 GPIO */
	GPIO_graceInit();

	/* initialize Config for the MSP430 2xx family clock systems (BCS) */
	BCSplus_graceInit();

	/* initialize Config for the MSP430 System Registers */
	System_graceInit();

	/* initialize Config for the MSP430 WDT+ */
	WDTplus_graceInit();

	for(;;)
	{
	   if((P1IN & PUSH_BTN_STATE) == !PUSH_BTN_STATE)
	   {
	      P1OUT |= GREEN_LED_STATE;
	      _delay_cycles(190000);
	      P1OUT &= ~GREEN_LED_STATE;
	   }
	   P1OUT ^= RED_LED_STATE;
	   _delay_cycles(160000);
	}
}

void BCSplus_graceInit(void)
{
    /* Basic Clock System Control 2
     *
     * SELM_0 -- DCOCLK (most reliable clock, moderate accuracy)
     * DIVM_0 -- Divide by 1
     * ~SELS -- DCOCLK
     * DIVS_0 -- Divide by 1
     * ~DCOR -- DCO uses internal resistor
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
    if (CALBC1_1MHZ != 0xFF) {
        /* Follow recommended flow. First, clear all DCOx and MODx bits. Then
         * apply new RSELx values. Finally, apply new DCOx and MODx bit values.
         */
        DCOCTL = 0x00;
        BCSCTL1 = CALBC1_1MHZ;      /* Set DCO to 1MHz */
        DCOCTL = CALDCO_1MHZ;
    }
    /*
     * Basic Clock System Control 1
     *
     * XT2OFF -- Disable XT2CLK
     * ~XTS -- Low Frequency
     * DIVA_0 -- Divide by 1
     *
     * Note: ~XTS indicates that XTS has value zero
     */
    BCSCTL1 |= XT2OFF | DIVA_0;
    /*
     * Basic Clock System Control 3
     *
     * XT2S_0 -- 0.4 - 1 MHz
     * LFXT1S_2 -- If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_1 -- ~6 pF
     */
    BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;

}

void GPIO_graceInit(void)
{
    /* Port 1 Output Register */
    P1OUT = BIT3;
    /* Port 1 Direction Register */
    P1DIR = BIT0 | BIT6;
    /* Port 1 Resistor Enable Register */
    P1REN = BIT3;
    /* Port 1 Interrupt Edge Select Register */
    P1IES = 0;
    /* Port 1 Interrupt Flag Register */
    P1IFG = 0;
    /* Port 2 Output Register */
    P2OUT = 0;
    /* Port 2 Direction Register */
    P2DIR = 0;
    /* Port 2 Interrupt Edge Select Register */
    P2IES = 0;
    /* Port 2 Interrupt Flag Register */
    P2IFG = 0;
}

void System_graceInit(void)
{
    /* Clear oscillator fault flag with software delay */
    do
    {
        // Clear OSC fault flag
        IFG1 &= ~OFIFG;
        // 50us delay
        __delay_cycles(50);
    } while (IFG1 & OFIFG);
    /*
     * SR, Status Register
     *
     * ~SCG1 -- Disable System clock generator 1
     * ~SCG0 -- Disable System clock generator 0
     * ~OSCOFF -- Oscillator On
     * ~CPUOFF -- CPU On
     * GIE -- General interrupt enable
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    __bis_SR_register(GIE);
    /* USER CODE START (section: System_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: System_graceInit_epilogue) */
}
void WDTplus_graceInit(void)
{
    /*
     * WDTCTL, Watchdog Timer+ Register
     *
     * WDTPW -- Watchdog password
     * WDTHOLD -- Watchdog timer+ is stopped
     * ~WDTNMIES -- NMI on rising edge
     * ~WDTNMI -- Reset function
     * ~WDTTMSEL -- Watchdog mode
     * ~WDTCNTCL -- No action
     * ~WDTSSEL -- SMCLK
     * ~WDTIS0 -- Watchdog clock source bit0 disabled
     * ~WDTIS1 -- Watchdog clock source bit1 disabled
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    WDTCTL = WDTPW | WDTHOLD;
    /* USER CODE START (section: RTC_B_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: RTC_B_graceInit_epilogue) */
}
