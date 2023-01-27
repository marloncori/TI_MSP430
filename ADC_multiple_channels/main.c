#include <msp430.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//drselim MSP430 & Multiple ADC
//Please don't forget to give credits while using the code
//video link for the description of the code:
//https://youtu.be/Ev871bhGFt0
char vt_chara0[5];
char vt_chara1[5];
char vt_chara2[5];
char vt_chara3[5];
char vt_chara4[5];
char vt_chara5[5];
char vt_chara6[5];
char vt_chara7[5];
char volta0[] = " A0:   ";
char volta1[] = " A1(NA): ";//used by UART no connection
char volta2[] = " A2(NA): ";//used by UART no connection
char volta3[] = " A3:   ";
char volta4[] = " A4:   ";
char volta5[] = " A5:   ";
char volta6[] = " A6:   ";
char volta7[] = " A7:   ";
char newline[] = " \r\n";
unsigned int adc[8];
void ser_output(char *str);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    P1SEL = BIT1|BIT2;
    P1SEL2 = BIT1|BIT2;

    UCA0CTL1 |= UCSWRST+UCSSEL_2;
    UCA0BR0 = 52; // 19600 baud rate settings
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST;

    ADC10CTL1 = INCH_7 + ADC10DIV_0 + CONSEQ_3 + SHS_0;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + MSC + ADC10ON; //ADC10IE
    ADC10AE0 = BIT7 + BIT6 + BIT5 + BIT4 + BIT3 + BIT0;
    ADC10DTC1 = 8;


    while(1){
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & BUSY);
        ADC10CTL0 |= ENC + ADC10SC;
        ADC10SA = (unsigned int)adc;
        //if you get an error with ltoa, try: 'ltoa(adc[7],vt_chara0,10)'
        //check article:
        //https://software-dl.ti.com/ccs/esd/documents/sdto_cgt_handling_changes_in_ltoa.html
        ltoa(adc[7],vt_chara0); ltoa(adc[6],vt_chara1); ltoa(adc[5],vt_chara2); //if you get an error with ltoa, try: 'ltoa(adc[7],vt_chara0,10)'
        ltoa(adc[4],vt_chara3); ltoa(adc[3],vt_chara4); ltoa(adc[2],vt_chara5);
        ltoa(adc[1],vt_chara6); ltoa(adc[0],vt_chara7);
        ser_output(volta0); ser_output(vt_chara0); ser_output(volta1); ser_output(vt_chara1);
        ser_output(volta2); ser_output(vt_chara2); ser_output(volta3); ser_output(vt_chara3);
        ser_output(volta4); ser_output(vt_chara4); ser_output(volta5); ser_output(vt_chara5);
        ser_output(volta6); ser_output(vt_chara6); ser_output(volta7); ser_output(vt_chara7);
        ser_output(newline);
        __delay_cycles(100000);
    }
    //drselim MSP430 & Multiple ADC
    //Please don't forget to give credits while using the code
    //video link for the description of the code:
    //https://youtu.be/Ev871bhGFt0
}

void ser_output(char *str){
    while(*str != 0){
        while (!(IFG2&UCA0TXIFG));
        UCA0TXBUF = *str++;
    }
}
