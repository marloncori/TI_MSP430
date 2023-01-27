#include <msp430.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/**
 * //Connections
   P2.1 ECHO
   P1.6 TRIGGER yellow

   Timers in MSP430 by drselim
   Please don't forget to give credits while sharing this code
    for the video description for the code:
    https://youtu.be/LzOVfDQaol8
 *
 */

// *********************************************************************************************
//  *** GLOBAL VARIABLES ***
// *********************************************************************************************
char printdist[] = "\n ---------------------------------------------\n  Distance to the nearest object is: ";
 char centimeter[] = " cm", dot[] = ".";
 char zerro[] = "0", newline[] = " \r\n";
 volatile int temp[2];
 volatile float diff;
 volatile unsigned int i=0;
 int dst_int, dst_flt;
 float tmp_flt;
 char dst_char[5], dst_flt_char[5];
 volatile float distance;

//**********************************************************************************************
//  *** FUNCTION PROTOTYPES ***
//***********************************************************************************************
void stop_watchdog_tmr(void);
void clk_config(void);
void gpio_setup(void);
void usart_init(void);
void tA0_settings(void);
void ser_output(char *str);
void show_result(char *dist, int mode);

//**********************************************************************************************
//  ::: MAIN ROUTINE :::
//***********************************************************************************************
void main(void){

    stop_watchdog_tmr();
	clk_config();
	
	gpio_setup();
    usart_init();

    tA0_settings();
    _enable_interrupts();
    LPM1;

    while(1){
        distance = diff/58;
        dst_int = floor(distance);
        tmp_flt = distance - dst_int;
        /*if you get an error with ltoa, try: 'ltoa(dst_int, dst_char,10)'
         check article:
         https://software-dl.ti.com/ccs/esd/documents/sdto_cgt_handling_changes_in_ltoa.html*/
        ltoa(dst_int, dst_char);
        if (tmp_flt < 0.01) {
            dst_flt = floor(tmp_flt * 1000);
            ltoa(dst_flt,dst_flt_char);
            show_result(dst_flt_char, 1);
        }
        else if (tmp_flt < 0.1) {
            dst_flt = floor(tmp_flt * 100);
            ltoa(dst_flt,dst_flt_char);
            show_result(dst_flt_char, 2);
        }
        else {
            dst_flt = floor(tmp_flt * 100);
            ltoa(dst_flt,dst_flt_char);
            show_result(dst_flt_char, 3);
        }
      ser_output(newline);
       __delay_cycles(500000);  //0.5 second delay
    }
}

//**********************************************************************************************
//  *** INTERRUPT SERVICE ROUTINE ***
//***********************************************************************************************
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void){
    temp[i] = TA1CCR1;
    i += 1;
    TA1CCTL1 &= ~CCIFG ;
    if (i==2) {
       diff = temp[i-1] - temp[i-2];
       i = 0;
    }
}

//**********************************************************************************************
//  ::: FUNCTION DEFINITIONS :::
//***********************************************************************************************
void stop_watchdog_tmr(void){
   WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}

void clk_config(void){
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
}

void gpio_setup(void){
    P1DIR = BIT6;   // P1.6 - TRIGGER pin
    P2SEL = BIT1;   // P2.1 - ECHO pin of sensor
    P1SEL = BIT1|BIT2|BIT6;
    P1SEL2 = BIT1|BIT2;
}

void usart_init(void){
    //settings for 19200 baud rate
    UCA0CTL1 |= UCSWRST+UCSSEL_2;
    UCA0BR0 = 52;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST;
}

void tA0_settings(void){
    TA0CTL = TASSEL_2|MC_1 ;
    TA0CCR0 = 0xFFFF;
    TA0CCR1 = 0x000A;
    TA0CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2|MC_2 ;
    TA1CCTL1 = CAP | CCIE | CCIS_0 | CM_3 | SCS ;
}

void ser_output(char *str){
    while(*str != 0) {
      while (!(IFG2 & UCA0TXIFG));
       UCA0TXBUF = *str++;
    }
}

void show_result(char* dist, int mode){
    ser_output(printdist);
    ser_output(dst_char);
    ser_output(dot);
    switch(mode){
     case 1:
        ser_output(zerro);
        ser_output(zerro);
        break;
    case 2:
        ser_output(zerro);
        break;
    case 3:
         break;
    }
    ser_output(dist);
    ser_output(centimeter);
}

// **********************************************************************************************
//   :: END OF PROGRAM :::
// ***********************************************************************************************
