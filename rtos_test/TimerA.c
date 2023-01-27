/*
 * TimerA.c
 *
 *  Created on: 2022. okt. 22.
 *      Author: NUC
 */
#include <TimerA.h>


#pragma vector=9
__interrupt void TIMER0_IRQ(void)
{

}

#pragma vector=8
__interrupt void TIMER01_IRQ(void)
{

    switch(_TA0IV)
    {
    case TAIV_CC1:
        break;
    case TAIV_CC2:
        break;
    case TAIV_IFG:
        break;
    default:
        break;
    }
}

#pragma vector=13
__interrupt void TIMER1_IRQ(void)
{

}

#pragma vector=12
__interrupt void TIMER11_IRQ(void)
{
    switch(_TA1IV)
    {
    case TAIV_CC1:
        break;
    case TAIV_CC2:
        break;
    case TAIV_IFG:
        break;
    default:
        break;
    }
}




