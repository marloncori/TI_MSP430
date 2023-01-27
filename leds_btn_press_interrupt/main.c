#include <msp430.h> 

#define BUTTON    BIT3
#define RED_LED   BIT0
#define GREEN_LED BIT6

static unsigned int counter = 0;

void stop_watchdog();
void GPIO_graceInit();
void BCSplus_graceInit();
void System_graceInit();
void WDTplus_graceInit();

void main(void)
{
    stop_watchdog();
    GPIO_graceInit();
    BCSplus_graceInit();
    System_graceInit();
    WDTplus_graceInit();

    _enable_interrupts();

    while(1); // wait for an interrupt
}

#pragma vector=PORT1_VECTOR
// define interrupt vector
__interrupt void PORT1_ISR(void)
{

    // Interrupt Service Routine
    counter++;
    switch(counter)
    {
        case 1:
            P1OUT = RED_LED;
            break;
        case 2:
            P1OUT = 0x00;
            P1OUT = GREEN_LED;
            break;
        case 3:
            P1OUT = 0x00;
            P1OUT = 0x41;
            break;
        case 4:
            P1OUT = 0x00;
            break;
        default:
            counter = 0;
            break;
    }
    //clear interrupt flag
    P1IFG = 0x00;
}
void stop_watchdog(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    // stop watchdog timer
}

void GPIO_graceInit(void)
{
    /* USER CODE START (section: GPIO_graceInit_prologue) */
    /* User initialization code */
    /* USER CODE END (section: GPIO_graceInit_prologue) */
    /* Port 1 Output Register */
    P1OUT = BUTTON;
    /* Port 1 Direction Register */
    P1DIR = RED_LED | GREEN_LED;
    /* Port 1 Resistor Enable Register */
    P1REN = BUTTON;
    /* Port 1 Interrupt Edge Select Register */
    P1IE   = BUTTON;
    // enable interrupt from port 1
    P1IES = BUTTON;
    /* Port 1 Interrupt Flag Register */
    P1IFG = 0;
    /* Port 2 Output Register */
    P2OUT = 0;
    /* Port 2 Port Select Register */
    P2SEL &= ~(BIT6 | BIT7);
    /* Port 2 Direction Register */
    P2DIR = 0;
    /* Port 2 Interrupt Edge Select Register */
    P2IES = 0;
    /* Port 2 Interrupt Flag Register */
    P2IFG = 0;
    /* USER CODE START (section: GPIO_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: GPIO_graceInit_epilogue) */
}
void BCSplus_graceInit(void)
{
    /* USER CODE START (section: BCSplus_graceInit_prologue) */
    /* User initialization code */
    /* USER CODE END (section: BCSplus_graceInit_prologue) */
    /*
     * Basic Clock System Control 2
     *
     * SELM_0 -- DCOCLK
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
     * DIVA_3 -- Divide by 8
     *
     * Note: ~XTS indicates that XTS has value zero
     */
    BCSCTL1 |= XT2OFF | DIVA_3;
    /*
     * Basic Clock System Control 3
     *
     * XT2S_0 -- 0.4 - 1 MHz
     * LFXT1S_2 -- If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_1 -- ~6 pF
     */
    BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;
    /* USER CODE START (section: BCSplus_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: BCSplus_graceInit_epilogue) */
}
void System_graceInit(void)
{
    /* USER CODE START (section: System_graceInit_prologue) */
    /* User initialization code */
    /* USER CODE END (section: System_graceInit_prologue) */
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
    /* USER CODE START (section: RTC_B_graceInit_prologue) */
    /* User initialization code */
    /* USER CODE END (section: RTC_B_graceInit_prologue) */
    /*
     * WDTCTL, Watchdog Timer+ Register
     *
     * WDTPW -- Watchdog password
     * ~WDTHOLD -- Watchdog timer+ is not stopped
     * ~WDTNMIES -- NMI on rising edge
     * ~WDTNMI -- Reset function
     * ~WDTTMSEL -- Watchdog mode
     * ~WDTCNTCL -- No action
     * WDTSSEL -- ACLK
     * ~WDTIS0 -- Watchdog clock source bit0 disabled
     * ~WDTIS1 -- Watchdog clock source bit1 disabled
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    WDTCTL = WDTPW | WDTSSEL;
    /* USER CODE START (section: RTC_B_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: RTC_B_graceInit_epilogue) */
}


