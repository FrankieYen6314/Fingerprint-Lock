#include "Beep.h"

/* Private function prototypes -----------------------------------------------*/
static void BeepSetTime_95us(uint16 xms);
static void BeepSetTime_100us(uint16 xms);
static void BeepSetTime_105us(uint16 xms);
static void BeepSetTime_112us(uint16 xms);


void BeepInit()
{
	P2DIR |= SPK;       //P2.3 output mode, used to drive buzzer
    P2OUT &= ~SPK;      //��·��NPN���������������͵�ƽʹ�����ܽ�ֹ���رշ�����
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
 *@brief	�÷����������Ե���һ��ʱ�䣬����һ��ʱ��
OnTime		____��λ��ms
OffTime		____��λ��ms
ToggleTime	____Ƶ�ʱ��
num			____����
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
˵��������__delay_cycles(); ����Ϊ���к������������ܸ�ֵΪ����������Ǳ����Ļ���
�����ʱ���ټ��ϱ������Ļ��ָ��Ͳ��ܾ�ȷ��ʱ�ˣ�����Ϊ�����Ŀ�ģ�����ֻ���Ǹ�
ֵΪ��������
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

