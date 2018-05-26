#include "Timer.h"
#include "Key.h"
#include "ADC.h"
#include "Led.h"
#include "Warning.h"

/* Public variables ----------------------------------------------------------*/
volatile bool g_bEnroll_Permission_Timeout =false;

/* Imported variables --------------------------------------------------------*/
extern char g_current_mode;
extern char g_previous_mode;


/* Private variables ---------------------------------------------------------*/
static bool bTimerA_KeyCheck		=false;
static bool bTimerA_WaitTime		=false;
static bool bTimerA_EnrollLED		=false;
static bool bTimerA_PermissionLED	=false;

static uint16 KeyCheck_Counter =0;
static uint16 TimeOut_Counter =0;
static uint16 EnrollLED_Counter =0;
static uint16 PermissionLED_Counter =0;


/* Private function prototype ------------------------------------------------*/
static void TimerA_SMCLK_Init();
static void TimerB_ACLK_Init();

static void TimerA_KeyCheck();
static void TimerA_WaitTime();
static void TimerA_EnrollLED();
static void TimerA_PermissionLED();


void TimerInit()
{
	TimerA_SMCLK_Init();
#ifndef NoSleepBatteryCheck
	TimerB_ACLK_Init();
#endif
}

/* Timer clock init ----------------------------------------------------------*/
static void TimerA_SMCLK_Init()
{
	TACCTL0 |= CCIE;	//Interrupt enabled
    TACTL |= TASSEL_2 + MC_1 + ID_0;	//SMCLK, up-mode, 1 input devider(1MHz)
}

static void TimerB_ACLK_Init()
{
	TA1CCTL0 |= CCIE;
    TA1CTL |= TASSEL_1 + MC_1 + ID_3;	//ACLK, up-mode, 1/8 input divider(1.5KHz)
}

/* Timer_A Operation ---------------------------------------------------------*/
void TimerA_Cmd(FunctionalState NewState)
{
	if(NewState !=DISABLE)	//Enable TimerA
	{
		TACCTL0 |= CCIE;    //Enable timerA interrupt
    	TACCR0 = 10000;     //10ms£¬16bit counter
	}
	else
	{
		TACCTL0 &= ~CCIE;
		TACCR0 = 0;
	}
}

/**
 *@brief	Enable Timer_A
 *@param	TaskBit: Specifies the task of Timer_A
 *			0x01--TimerA_KeyCheck();
 *			0x02--TimerA_WaitTime();
 *			0x04--TimerA_EnrollLED();
 *			0x08--TimerA_PermissionLED();
 *			You can choose any combination of the tasks above.
 */
void TimerA_Start(char TaskBit)
{
	TimerA_TaskCounterRefresh(TaskBit);
	TimerA_TaskCmd(TaskBit, ENABLE);
	TimerA_TaskCmd(~TaskBit, DISABLE);
	TimerA_Cmd(ENABLE);
}

/**
 *@brief	Disable Timer_A
 *
 */
void TimerA_Stop()
{
	TimerA_Cmd(DISABLE);
	TimerA_TaskCmd(0x0f, DISABLE);
}

/**
 *@brief	Enable Task of Timer_A
 *@param	TaskBit: Specifies the task of Timer_A
 *			0x01 -- TimerA_KeyCheck();
 *			0x02 -- TimerA_WaitTime();
 *			0x04 -- TimerA_EnrollLED();
 *			0x08 -- TimerA_PermissionLED();
 *			You can choose any combination of the tasks above.
 *@param	NewState: specifies the new state of beep.
 *			This parameter can be one of ENABLE or DISABLE.
 */
void TimerA_TaskCmd(char TaskBit, FunctionalState NewState)
{
	if(NewState !=DISABLE)	//Enable
	{
		if(TaskBit &0x01) bTimerA_KeyCheck =true;
		if(TaskBit &0x02) bTimerA_WaitTime =true;
		if(TaskBit &0x04) bTimerA_EnrollLED =true;
		if(TaskBit &0x08) bTimerA_PermissionLED =true;
	}
	else	//Disable
	{
		if(TaskBit &0x01) bTimerA_KeyCheck =false;
		if(TaskBit &0x02) bTimerA_WaitTime =false;
		if(TaskBit &0x04) bTimerA_EnrollLED =false;
		if(TaskBit &0x08) bTimerA_PermissionLED =false;
	}
}
/**
 *@brief	Refresh the counter of corresponding task of Timer_A.
 *@param	TaskBit: Specifies the task of Timer_A
 *			0x01 -- TimerA_KeyCheck();
 *			0x02 -- TimerA_WaitTime();
 *			0x04 -- TimerA_EnrollLED();
 *			0x08 -- TimerA_PermissionLED();
 *			You can choose any combination of the tasks above.
 */
void TimerA_TaskCounterRefresh(char TaskBit)
{
	if(TaskBit &0x01) KeyCheck_Counter =0;
	if(TaskBit &0x02) TimeOut_Counter =0;
	if(TaskBit &0x04) EnrollLED_Counter =0;
	if(TaskBit &0x08) PermissionLED_Counter =0;
}

/* TimerB operation ----------------------------------------------------------*/
void TimerB_Cmd(FunctionalState NewState)
{
	if(NewState !=DISABLE)	//Enable TimerB
	{
		TA1CCTL0 |= CCIE;
		TA1CCR0 = 30000;	//20s(1500x20 --> 1sx20)
	}
	else
	{
		TA1CCTL0 &= ~CCIE;
    	TA1CCR0 = 0;
	}
}

/**
 * @brief	Timer_A Interrupt Service Request function
 *			This is used for different TimerA tasks
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A ()
{
	if(bTimerA_KeyCheck)
		TimerA_KeyCheck();
	if(bTimerA_WaitTime)
		TimerA_WaitTime();
	if(bTimerA_EnrollLED)
		TimerA_EnrollLED();
	if(bTimerA_PermissionLED)
		TimerA_PermissionLED();
}

static void TimerA_KeyCheck()
{
	KeyCheck_Counter ++;
	if(BUTTON ==KeyScan()){
		if(KeyCheck_Counter >(LONG_PRESS_TIME/10)){
			TimerA_Stop();
			Warning_Delete_Start();
			g_previous_mode ='D';
			g_current_mode ='P';
		}
	}
	else {
		if(KeyCheck_Counter <(LONG_PRESS_TIME/10)){
			TimerA_Stop();
			Warning_Enroll_Start();
			g_previous_mode ='E';
			g_current_mode ='P';
		}
	}
}

static void TimerA_WaitTime()
{
	if(++TimeOut_Counter ==ENROLL_PERMI_TIME/10)
	{
		TimerA_Stop();
		TimeOut_Counter =0;
		Led_Off(LED_BLUE);
		Led_Off(LED_RED);
		g_bEnroll_Permission_Timeout =true;
	}
}

static void TimerA_EnrollLED()
{
	if(++EnrollLED_Counter ==LED_FLASH_TIME/10)
	{
		EnrollLED_Counter =0;
		Led_Toggle(LED_BLUE);
	}
}

static void TimerA_PermissionLED()
{
	if(++PermissionLED_Counter ==LED_FLASH_TIME/10)
	{
		PermissionLED_Counter =0;
		Led_Toggle(LED_RED);
	}
}


/**
 * @brief	TimerB Interrupt Service Request function
 *			This is used for regular battery voltage check
 */
#if SleepBatteryCheck
static uint16 VLO_Counter =0;

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_B ()	//generate an interrupt request every 20s
{
	VLO_Counter ++;
	if(VLO_Counter ==(3* 60* BATTERY_TEST_HOUR))
	{
		TimerB_Cmd(DISABLE);
		ExternalInterruptCmd(DISABLE);
		VLO_Counter =0;
		BatteryCheck();
		ExternalInterruptCmd(ENABLE);
		TimerB_Cmd(ENABLE);
	}
}
#endif
