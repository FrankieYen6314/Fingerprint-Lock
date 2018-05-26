#ifndef _TIMER_H_
#define _TIMER_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

/* Exported Functions --------------------------------------------------------*/
void TimerInit();
void TimerA_Cmd(FunctionalState NewState);
void TimerA_Start(char TaskBit);
void TimerA_Stop();
void TimerA_TaskCmd(char TaskBit, FunctionalState NewState);
void TimerA_TaskCounterRefresh(char TaskBit);
void TimerB_Cmd(FunctionalState NewState);

#endif