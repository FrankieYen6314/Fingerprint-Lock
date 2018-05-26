#include "Led.h"

void LedInit()
{
	P2SEL &= ~(BIT6 + BIT7);
        //P2.6��P2.7����Ĭ������XIN��XOUT����Ҫ��ΪI/O�ڣ�������P2SEL�Ĵ�����BIT6��BIT7���ο�Datasheet P46ҳ
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


