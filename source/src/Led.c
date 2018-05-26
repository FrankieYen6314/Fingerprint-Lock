#include "Led.h"

void LedInit()
{
	P2SEL &= ~(BIT6 + BIT7);
        //P2.6和P2.7引脚默认用作XIN和XOUT，想要作为I/O口，需设置P2SEL寄存器的BIT6和BIT7，参考Datasheet P46页
    P2DIR |= (LED_R + LED_B);
    P2OUT &= ~(LED_R + LED_B);
}

/**
  *@brief	Turns selected LED on.
  *@param	LedColor: Specifies the LED to be set on.
  * 	This parameter can be one of LED_BLUE and LED_RED.
  *@retval	None
  */
void Led_On(Led_TypeDef LedColor)
{
	if(LED_BLUE == LedColor)
		P2OUT |=LED_B;
	else if(LED_RED == LedColor)
		P2OUT |=LED_R;
}

/**
  *@brief	Turns selected LED off.
  *@param	LED: Specifies the LED to be set off.
  * 	This parameter can be one of LED_BLUE and LED_RED.
  *@retval	None
  */
void Led_Off(Led_TypeDef LedColor)
{
	if(LED_BLUE == LedColor)
		P2OUT &= ~LED_B;
	else if(LED_RED == LedColor)
		P2OUT &= ~LED_R;
}

/**
  *@brief	Turns selected LED on or off.
  *@Param	LedColor: Specifies the LED to be toggled.
  * 	This parameter can be one of LED_BLUE and LED_RED.
  *@retval	None
  */
void Led_Toggle(Led_TypeDef LedColor)
{
	if(LED_BLUE == LedColor)
		P2OUT ^= LED_B;
	else if(LED_RED == LedColor)
		P2OUT ^= LED_R;
}


