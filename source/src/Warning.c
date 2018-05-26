#include "Warning.h"
#include "Led.h"
#include "Beep.h"
#include "Timer.h"
#include "Key.h"


uint16 Warning_ConnectError()
{
	Led_Off(LED_RED);
	for(uint8 i =0; i<5; i++)
	{
		for(uint8 j =0; j<4; j++)
		{
			Led_Toggle(LED_RED);
			delayms(200);
		}
		BeepSetTime(BeepToggle_100us, 300);
	}
	Led_On(LED_RED);
	delayms(2000);
	Led_Off(LED_RED);
	
	return 7500;
}

/* Warning--verify -----------------------------------------------------------*/
uint16 Warning_Verify_Pass()
{
	Led_On(LED_BLUE);
	BeepSetTime(BeepToggle_112us, 60);
	BeepSetTime(BeepToggle_95us, 60);
	Led_Off(LED_BLUE);
	
	return 120;
}

uint16 Warning_Verify_FirstUseFail()
{
	Led_Off(LED_BLUE);
	Led_On(LED_RED);
	delayms(3000);
	Led_Off(LED_RED);
	
	return 3000;
}

uint16 Warning_Verify_Fail()
{
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_100us, 400);
	Led_Off(LED_RED);
	
	return 400;
}

uint16 Warning_Verify_RingTouch()
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	
	return 0;
}

/* Warning--enroll -----------------------------------------------------------*/
uint16 Warning_Enroll_Start()
{
	BeepSetTime(BeepToggle_100us, 100);
	delayms(100);
	
	return 200;
}

uint16 Warning_Enroll_Success()
{
	Led_On(LED_BLUE);
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_112us, 120);
	BeepSetTime(BeepToggle_105us, 110);
	BeepSetTime(BeepToggle_100us, 100);
	BeepSetTime(BeepToggle_95us, 150);
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	
	return 480;
}

uint16 Warning_Enroll_Fail()
{
	BeepSetTime(BeepToggle_100us, 100);
	delayms(100);
	BeepSetTime(BeepToggle_100us, 100);
	
	return 300;
}

uint16 Warning_Enroll_BadImage()
{
	Led_Off(LED_BLUE);
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_100us, 500);
	Led_Off(LED_RED);
	
	return 500;
}

uint16 Warning_Enroll_QualifiedImage()
{
	Led_Off(LED_BLUE);
	for(uint8 i =0; i<4; i++)
	{
		Led_Toggle(LED_BLUE);
		delayms(100);
	}
	Led_On(LED_BLUE);
	BeepSetTime(BeepToggle_100us, 100);
	Led_Off(LED_BLUE);
	
	return 500;
}

uint16 Warning_Enroll_MergeFail()
{
	return Warning_Verify_Fail();
}

uint16 Warning_Enroll_TimeOut()
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	
	return 0;
}

/* Warning--delete -----------------------------------------------------------*/
uint16 Warning_Delete_Start()
{
	BeepSetTime(BeepToggle_100us, 400);
	
	return 400;
}

uint16 Warning_Delete_Fail()
{
	Led_Off(LED_BLUE);
	Led_On(LED_RED);
	delayms(500);
	BeepSetTime(BeepToggle_100us, 1000);
	Led_Off(LED_RED);
	
	return 1500;
}

uint16 Warning_Delete_Success()
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	for(uint8 i =0; i<6; i++)
	{
		Led_Toggle(LED_RED);
		delayms(500);
	}
	Led_On(LED_RED);
	delayms(500);
	Led_On(LED_BLUE);
	BeepSetTime(BeepToggle_100us, 500);
	delayms(500);
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	
	return 4500;
}

/* Warning--permission -------------------------------------------------------*/
uint16 Warning_Permission_Fail()
{
	Led_Off(LED_BLUE);
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_100us, 500);
	Led_Off(LED_RED);
	
	return 500;
}

uint16 Warning_Permission_Pass()
{
	Led_Off(LED_RED);
	Led_Off(LED_BLUE);
	for(uint8 i =0; i<4; i++)
	{
		Led_Toggle(LED_BLUE);
		delayms(200);
	}
	Led_On(LED_BLUE);
	BeepSetTime(BeepToggle_112us, 100);
	BeepSetTime(BeepToggle_95us, 100);
	delayms(500);
	Led_Off(LED_BLUE);
	
	return 1500;
}

uint16 Warning_Enroll_FullSpace()
{
	Led_Off(LED_BLUE);
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_100us, 150);
	delayms(200);
	BeepSetTime(BeepToggle_100us, 150);
	Led_Off(LED_RED);
	
	return 500;
}

uint16 Warning_Permission_TimeOut()
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	
	return 0;
}

/* Warning--Lock -------------------------------------------------------------*/
uint16 Warning_LockOpen()
{
	Warning_Verify_Pass();
	
	return 120;
}

uint16 Warning_LockClose()
{
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_105us, 120);
	Led_Off(LED_RED);
	
	return 120;
}

/* Warning--Battery ----------------------------------------------------------*/
uint16 Warning_LowVoltage()
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	for(uint8 i =0; i<5; i++)
	{
		Led_On(LED_RED);
		delayms(700);
		Led_Off(LED_RED);
		delayms(300);
	}
	Led_On(LED_RED);
	BeepSetTime(BeepToggle_100us, 500);
	delayms(500);
	Led_Off(LED_RED);
	
	return 6000;
}

uint16 Warning_BadVoltage() //ºôÎüµÆ£¿£¿
{
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
	for(uint8 i =0; i<10; i++)
	{
		Led_Toggle(LED_RED);
		delayms(1000);
	}
	Led_Off(LED_RED);
	
	return 10000;
}

/* LED Control ---------------------------------------------------------------*/
void LED_UserCancel()
{
	TimerA_Cmd(DISABLE);
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
}

void LED_Enroll_WaitForFP()
{
	TimerA_Cmd(ENABLE);
	InterruptEnable(BUTTON);
}

void LED_Enroll_GetFP()
{
	TimerA_Cmd(DISABLE);
	ExternalInterruptCmd(DISABLE);
	Led_On(LED_BLUE);
}

void LED_Permission_WaitForFP()
{
	TimerA_Cmd(ENABLE);
	InterruptEnable(BUTTON);
}

void LED_Permission_GetFP()
{
	TimerA_Cmd(DISABLE);
	ExternalInterruptCmd(DISABLE);
	Led_On(LED_RED);
}