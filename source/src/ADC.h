#ifndef _ADC_H_
#define _ADC_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

/* Exported Functions --------------------------------------------------------*/
void ADC_Init();
uint16 ADC_Battery_Convert();

#endif