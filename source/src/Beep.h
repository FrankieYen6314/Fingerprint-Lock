#ifndef _BEEP_H_
#define _BEEP_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"


typedef enum
{
	BeepToggle_95us,		//��ƽ��ת����95us
	BeepToggle_100us,		//��ƽ��ת����100us
	BeepToggle_105us,		//��ƽ��ת����105us
	BeepToggle_112us		//��ƽ��ת����112us
}BeepToggle_TypeDef;

/* Exported Functions --------------------------------------------------------*/
void BeepInit();
void BeepCmd(FunctionalState NewState);
void BeepStop();
void BeepSetTime(BeepToggle_TypeDef ToggleTime, uint16 xms);
void BeepOnOffTime(uint16 OnTime, uint16 OffTime, BeepToggle_TypeDef ToggleTime, uint8 num);



#endif