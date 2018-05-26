#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

/* Exported Functions --------------------------------------------------------*/
void MotorInit();
void MotorForward();
void MotorReverse();
void MotorKeep();
void MotorStandby();

#endif