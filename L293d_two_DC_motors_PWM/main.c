#include <msp430.h> 

/**
 * --------------------------------------------------------------------
 * Motor driver connections to the MSP430G2553:
 * P2.0  -> PWM_SPEED_CTL_RM (RM = right motor)
 * P2.1  -> MOTOR_A1
 * P2.2  -> MOTOR_A2
 * P2.3  -> MOTOR_B1
 * P2.4  -> MOTOR_B2
 * P2.6 -> PWM_SPEED_CTL_LM (LM = left motor)
 * main.c
 * ---------------------------------------------------------------------
 * DC motor connections to motor driver L293D:
 * pin1 -> PWM_SPEED_CTL_RM
 * pin2 (input) -> MOTOR_A1
 * pin3 [output] motor a1
 * pin4 GND
 * pin5 GND
 * pin6 [output] motor a2
 * pin7 (input) -> MOTOR_A2
 * pin8 Vcc {12 v of motors through a 100uf capacitor}
 * pin9  -> PWM_SPEED_CTL_LM
 * pin10 (input) -> MOTOR_B1
 * pin11 [output] motor b1
 * pin12 GND
 * pin13 GND
 * pin14 [output] motor b2
 * pin15 (input) -> MOTOR_B2
 * pin16 Vcc 5v
 * -----------------------------------------------------------------------
 *
 * WARNING:
 *     P2.6 is Xin (crystal input pin) by default so to access the port
 *     function you have to explicitly clear the registers in P2SEL and P2SEL2.
 */

//*********************************************************************************************
void Delay(unsigned int count);
void Stop_Watchdog_Timer(void);
void PortFunction_Enable(void);
void LeftMotor_Init(void);
void Rotate_Clockwise(void);
void Stop_Motor(void);
void Rotate_Counter_Clockwise(void);
void Motor_Cleanup(void);
//*********************************************************************************************

void main(void)
{
    Stop_Watchdog_Timer();

	PortFunction_Enable();

	LeftMotor_Init();
	
	Rotate_Clockwise();
	Stop_Motor();

	Rotate_Counter_Clockwise();
	Motor_Cleanup();
}

//*********************************************************************************************
void Delay(unsigned int count){
  volatile unsigned int i;
  for(count=10;count>0;count--){
      for(i=0xFFFF;i>0;i--){
      }
  }
}
//*********************************************************************************************
void Stop_Watchdog_Timer(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}
//*********************************************************************************************
void PortFunction_Enable(void){
    P2SEL  &= ~BIT6; // clear P2.6 so as to be accessible for port function
    P2SEL2 &= ~BIT6;
}
//*********************************************************************************************
void LeftMotor_Init(void){
    P2DIR  |= BIT3 + BIT4 + BIT6; // P2.3, P2.4, P2.6 as output
    P2OUT  &= ~BIT3 + BIT4 + BIT6; // clear port
}
//*********************************************************************************************
void Rotate_Clockwise(void){
    P2OUT |= BIT3; // P2.3 = 1,P2.4 = 0
    P2OUT |= BIT6; // P2.6 = 1 ,3&4_EN = 1,Motor is started
    Delay(10);
}
//*********************************************************************************************
void Stop_Motor(void){
    // Stop the motor
    P2OUT &= ~BIT3 + BIT4; // P2.3 = 0,P2.4 = 0
    Delay(5);
}
//*********************************************************************************************
void Rotate_Counter_Clockwise(void){
    // Rotate the Motor Counter clockwise
    P2OUT &= ~BIT3; // P2.3 = 0
    P2OUT |=  BIT4; // P2.4 = 1
    P2OUT |= BIT6;  // P2.6 = 1 ,3&4_EN = 1,Motor is started
    Delay(10);      // Rotate motor for sometime
}
//*********************************************************************************************
void Motor_Cleanup(void){
    P2OUT &= ~BIT6; // P2.6 = 0,3&4_EN = 0,Motor is stoped
}
//*********************************************************************************************
