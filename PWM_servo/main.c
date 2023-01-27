#include <msp430.h> 


/**
 * Servo motors are controlled by PWM (Pulse Width Module).
 * Most of Servo motors works on 50Hz in frequency. The time duration for PWM
 * can be calculated as below:

   Time=1/Frequency
   T=1/50 -> T=20ms

 *  So to control a servo motor we need to have a 50Hz or 20ms PWM.
 * After setting the duration we need to set the necessary duty cycle of PWM.
 * This duty cycle will control the position of the servo motor.

 * For example the servo that I use works in 50Hz and duty cycle between 0.5ms to 1.5ms.
 * Let us see what is that means. I want to have a PWM with 50Hz duration and 50% duty cycle.
 * That means I need 20ms long signal duration, which is 10ms on and 10ms off.
 *
 * Now my servo motor need 50Hz  and a range between 0.5ms for zero degrees
 * and 1.5ms for 180 degrees.
 *
 * Any other position that we want to reach has a linear relation to these numbers. if you put your desired position into this equation you will get the duty cycle in ms;

 * DutyCycle=((1/180)*degree)+0.5
 *
 */

volatile unsigned int dutyCycle = 1650;

void main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR = BIT6; // set P1.6 as output
    P1SEL = BIT6; // set peripheral function which is TA1

    //set timer values
    TACTL = TASSEL_2 + ID_0 + MC_1 + TACLR + TAIFG;
    /* compare mode, SMCLK as source, input divider -> /1
     * mode control is up mode, counts up to TACCR0 value */

    TACCR0 = 2200;
    /* MCU_FREQ / SERVO_FREQ -> 1,100,000 / 50 = 2200 */

    TACCTL1 = OUTMOD_6; // set the timer mode as toggle/set to generate PWM
    TACCR1 = dutyCycle; // set duty cycle, PWM 0 = 550, 180=2750

    while(1){
        {
            dutyCycle = 1250; // set servo motor to 90 degrees
        }
    }
}
