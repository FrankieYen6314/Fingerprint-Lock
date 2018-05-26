#ifndef _KEY_H_
#define _KEY_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

#define	BUTTON	1
#define	RING	2

/* Exported Functions --------------------------------------------------------*/
void KeyInit();
uint8 KeyScan();
FlagStatus GetITStatus(uint8 key);
void ExternalInterruptCmd(FunctionalState NewState);
void InterruptEnable(uint8 key);



#endif