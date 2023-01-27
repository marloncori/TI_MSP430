#include <msp430.h> 

#define GREEN_LED BIT6
#define RED_LED   BIT0
/*
 * the MSP430 receives the password through the UART mode from the
host computer. The connection diagram for this application is given in Fig. 12.5. If the
password is correct, then the green LED on the MSP430 Launch-Pad turns on for 5 s, then
the code is reset. Otherwise, the red LED on the MSP430 LaunchPad turns on for 2 s, then
the MSP430 waits for the new password. Meanwhile, the MSP430 will tell the user to
enter the password and will determine whether the entered password is correct or not
though the terminal program.
 * */

char password[] = "Mc0489921";
char enter[] = "Enter your password: \r\n";
char correct[] = "The password is right!\r\n";
char incorrect[] = "The password is not correct\r\n";
char retry[] = "Please reenter your password\r\n";
char input[100];

unsigned int RX_byte_ctr = 0;
unsigned int cnt = 0;
unsigned int input_length;
unsigned int passwd_length;
unsigned int difference;

void transmit(char *str);
unsigned int compare(char *str, char *strpass);
unsigned int array_length(char *str);

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ; // adjust the clock
	DCOCTL = CALDCO_1MHZ;

	P1DIR  = RED_LED | GREEN_LED;
	P1OUT  = 0x00;
	P1SEL  = BIT1 | BIT2;
	P1SEL2 = BIT1 | BIT2;

	UCA0CTL1 |= UCSWRST + UCSSEL_2;
	// setup the uart mode, enable SW reset, use SMCLK
	UCA0BR0 = 104;
	// low bit of UCBRx is 104
	UCA0BR1 = 0;
	// high bit of UCBRx is 0
	UCA0MCTL = UCBRS_1;
	//second modulation stage select is 1
	//baud rate = 9600
	UCA0CTL1 &= ~UCSWRST;
	//clear SW reset, resume operation

	transmit(enter);
	
	IE2 |= UCA0RXIE;
	//enable USCI_A0 RX interrupt

	_enable_interrupts();

	while(1){
	    if(cnt == 1){
	        input_length = array_length(input);
	        difference = compare(input, password);
	        if(difference == 0){
	            //check if they match
	            transmit(correct);
	            P1OUT = GREEN_LED;
	            __delay_cycles(5000000);
	            //wait for 5 seconds
	            WDTCTL = WDT_MRST_0_064;
	            //reset the system
	        }
	        else {
	            //if they do not match
	            transmit(incorrect);
	            P1OUT = RED_LED;
	            __delay_cycles(2000000);
	            //wait for 2 seconds
	            P1OUT = 0x00;
	            transmit(retry);
	        }
	        cnt = 0;
	        RX_byte_ctr = 0;
	    }
	}
}

//USCI A receiver interrupt
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    // check if UCA0RXBUF is different from 0x0A;
    // (enter key from keyboard)
    if(UCA0RXBUF != 0x0A){
        input[RX_byte_ctr++] = UCA0RXBUF;
        //if it is, load received char
        //to current input string element
    }
    else {
       cnt = 1;
       input[RX_byte_ctr] = 0;
       //add null char at the end of input string
    }
}

void transmit(char *str){
    while(*str != 0){
        //do this during current element is
        // not equal to null character
        while(!(IFG2 & UCA0TXIFG));
        //ensure that transmit interrupt flag is set
        UCA0TXBUF = *str++;
    }
}

unsigned int compare(char *str, char *strpass){
    unsigned int result = 0;
    if(input_length <= passwd_length){
        while(*strpass != 0){
            result = result + abs((*str++)-(*strpass++));
            //if password length is greater, take the difference
            //between elements of strin and strpass until current
            //elements of strin nd strpass
        }
    }
     else {
        while(*str != 0){
            result = result + abs((*str++)-(*strpass++));
        }
    }
    return result;
}

unsigned int array_length(char *str){
    unsigned int length = 0;
    while(*str != 0){
        //until null char is reached
        str++;
        //increase array address
        length++;
        //increase length value
    }
    return length;
}
