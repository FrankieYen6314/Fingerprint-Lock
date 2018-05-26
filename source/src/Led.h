#ifndef _LED_H_
#define _LED_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

#define LED_BLUE	0
#define LED_RED		1

typedef int	Led_TypeDef;

/* Exported Functions --------------------------------------------------------*/
void LedInit();
void Led_On(Led_TypeDef LedColor);
void Led_Off(Led_TypeDef LedColor);
void Led_Toggle(Led_TypeDef LedColor);

#endif