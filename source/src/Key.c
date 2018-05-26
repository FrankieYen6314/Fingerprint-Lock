#include "Key.h"

void KeyInit()
{
	/*--------- Configure Switches(Enroll & Verify) ---------*/
    P2REN |= (V_BTN+E_BTN);    //P2.1 & P2.2 Enable pullup/pulldown resistor
    P2OUT |= E_BTN;            //P2.1 is Pulled up
    P2OUT &= ~V_BTN;           //P2.2 is pulled down
    P2IE  |= (E_BTN+V_BTN);    //P2.1 & P2.2 interrupt enabled
    P2IES |= E_BTN;            //P2.1 High/low edge
    P2IES &= ~V_BTN;           //P2.2 Low/High edge
    P2IFG &= ~(E_BTN+V_BTN);   //P2.1 & P2.2 IFG cleared just in case
	ExternalInterruptCmd(DISABLE);
}

uint8 KeyScan()
{
	uint8 key =0;
	
	if(!(P2IN & E_BTN))
		key =BUTTON;
	else if(P2IN & V_BTN)
		key =RING;
	return key;
}


FlagStatus GetITStatus(uint8 key)
{
	FlagStatus bitStatus =RESET;
	
	if(BUTTON ==key)
	{
		if(P2IFG & E_BTN)	bitStatus =SET;
	}
	else if(RING ==key)
	{
		if(P2IFG & V_BTN)	bitStatus =SET;
	}
	else bitStatus =RESET;
	
	return bitStatus;
}

/**
  *@brief	Enables or disables the external interrupt of BUTTON and RING.
  *@param	NewState: specifies the new state of external interrupt.
  *			This parameter can be one of ENABLE or DISABLE.
  *@retval	None
  */
void ExternalInterruptCmd(FunctionalState NewState)
{
	if(NewState !=DISABLE)
	{
		P2IFG &= ~(E_BTN+V_BTN);	//IFG cleared just in case,interrupt enabled
		P2IE |= (E_BTN+V_BTN);
	}
	else
	{
		P2IE &= ~(E_BTN+V_BTN);
	}
}

/**
  *@brief	Enables the external interrupt of BUTTON or RING.
  *@param	key: specifies the new key.
  *			This parameter can be one of BUTTON or RING.
  *@retval	None
  */
void InterruptEnable(uint8 key)
{
	if(BUTTON ==key)
	{
		P2IFG &= ~(E_BTN+V_BTN);
		P2IE |= E_BTN;
	}
	else if(RING ==key)
	{
		P2IFG &= ~(E_BTN+V_BTN);
		P2IE |= V_BTN;
	}
		
}