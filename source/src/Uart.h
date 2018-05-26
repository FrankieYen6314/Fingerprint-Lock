#ifndef _UART_H_
#define _UART_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"


/* Exported Functions --------------------------------------------------------*/
void UartInit();
void SerialPutChar(uint8 Data);


#endif