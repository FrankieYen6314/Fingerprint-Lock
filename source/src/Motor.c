#include "Motor.h"

void MotorInit()
{
	P2DIR |= (MOT_INA + MOT_INB);
    P2OUT &= ~(MOT_INA + MOT_INB);
}

/* Motor Control -------------------------------------------------------------*/
void MotorForward()
{
	P2OUT |= MOT_INA;	//HIGH
    P2OUT &= ~MOT_INB;	//LOW
}

void MotorReverse()	//Unlock
{
    P2OUT &= ~MOT_INA;	//LOW
    P2OUT |= MOT_INB;	//HIGH
}

void MotorKeep()	//Keep the previous mode(Forward/Reverse)
{
    P2OUT |= MOT_INA;	//HIGH
    P2OUT |= MOT_INB;	//LOW
}

void MotorStandby()
{
    P2OUT &= ~MOT_INA;	//LOW
    P2OUT &= ~MOT_INB;	//LOW
}