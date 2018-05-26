#ifndef _BEEP_H_
#define _BEEP_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"


typedef enum
{
	BeepToggle_95us,		//电平翻转周期95us
	BeepToggle_100us,		//电平翻转周期100us
	BeepToggle_105us,		//电平翻转周期105us
	BeepToggle_112us		//电平翻转周期112us
}BeepToggle_TypeDef;

/* Exported Functions --------------------------------------------------------*/
void BeepInit();
void BeepCmd(FunctionalState NewState);
void BeepStop();
void BeepSetTime(BeepToggle_TypeDef ToggleTime, uint16 xms);
void BeepOnOffTime(uint16 OnTime, uint16 OffTime, BeepToggle_TypeDef ToggleTime, uint8 num);



#endif