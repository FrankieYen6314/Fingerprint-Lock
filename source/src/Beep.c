#include "Beep.h"

/* Private function prototypes -----------------------------------------------*/
static void BeepSetTime_95us(uint16 xms);
static void BeepSetTime_100us(uint16 xms);
static void BeepSetTime_105us(uint16 xms);
static void BeepSetTime_112us(uint16 xms);


void BeepInit()
{
	P2DIR |= SPK;       //P2.3 output mode, used to drive buzzer
    P2OUT &= ~SPK;      //电路中NPN管驱动蜂鸣器，低电平使三极管截止，关闭蜂鸣器
}

/**
  *@brief	Enables or disables the beep.
  *@param	NewState: specifies the new state of beep.
  *			This parameter can be one of ENABLE or DISABLE.
  *@retval	None
  */
void BeepCmd(FunctionalState NewState)
{
	if(NewState !=DISABLE)
	{
		P2OUT |=SPK;
	}
	else
	{
		P2OUT &= ~SPK;
	}
}

void BeepStop()
{
	P2OUT &= ~SPK;
}

/**
  *@brief	Enables the beep for some time with a specific frequency level.
  *@param	ToggleTime: specifies the frequency level.
  *			This parameter can be one of the BeepToggle_TypeDef enum values.
  *@retval	None
  */
void BeepSetTime(BeepToggle_TypeDef ToggleTime, uint16 xms)
{
	switch(ToggleTime)
	{
		case BeepToggle_95us:
			BeepSetTime_95us(xms);
			break;
		case BeepToggle_100us:
			BeepSetTime_100us(xms);
			break;
		case BeepToggle_105us:
			BeepSetTime_105us(xms);
			break;
		case BeepToggle_112us:
			BeepSetTime_112us(xms);
			break;
		default:
			break;
	}
}

/*------------------------------------------------------------------------------
 *@brief	让蜂鸣器周期性的响一段时间，再灭一段时间
OnTime		____单位：ms
OffTime		____单位：ms
ToggleTime	____频率编号
num			____次数
------------------------------------------------------------------------------*/
void BeepOnOffTime(uint16 OnTime, uint16 OffTime, BeepToggle_TypeDef ToggleTime, uint8 num)
{
	for(uint8 i=0; i<num; i++)
	{
		BeepSetTime(ToggleTime, OnTime);
		delayms(OffTime);
	}
}

/* Set different frequency level -----------------------------------------------
说明：由于__delay_cycles(); 函数为固有函数，参数不能赋值为变量，如果是变量的话，
编译的时候再加上编译器的汇编指令就不能精确延时了，所以为了这个目的，必须只能是赋
值为立即数。
------------------------------------------------------------------------------*/
static void BeepSetTime_95us(uint16 xms)
{
	uint16 j =(uint16)((uint32)xms*1000/95);
	for(uint16 i=0; i<j; i++)
	{
		P2OUT ^= SPK;
		__delay_cycles(95);
	}
	BeepStop();
}

static void BeepSetTime_100us(uint16 xms)
{
	uint16 j =(uint16)((uint32)xms*1000/100);
	for(uint16 i=0; i<j; i++)
	{
		P2OUT ^= SPK;
		__delay_cycles(100);
	}
	BeepStop();
}

static void BeepSetTime_105us(uint16 xms)
{
	uint16 j =(uint16)((uint32)xms*1000/105);
	for(uint16 i=0; i<j; i++)
	{
		P2OUT ^= SPK;
		__delay_cycles(105);
	}
	BeepStop();
}

static void BeepSetTime_112us(uint16 xms)
{
	uint16 j =(uint16)((uint32)xms*1000/112);
	for(uint16 i=0; i<j; i++)
	{
		P2OUT ^= SPK;
		__delay_cycles(112);
	}
	BeepStop();
}

