/************************************************************************************
 * This project is used for the suitcase lock designed with the 536 FPM from Maocheng.
 *
 * IDE£ºIAR Embedded Workbench 430. V5.30
 * MCU: MSP430G2433(TI, 16 bit, RISC Architecture, 8K Flash)
 * CLOCK: configure the internal system clock to 1MHz
 *
 * Hardware: Zhang Weiqing
 * Software: Frankie Yen
 *
 * A-Kelon Optronics, Version 1.0
 *														   ____Frankie Yen 2017.10.12
************************************************************************************/

#include "Config.h"
#include "Motor.h"
#include "Key.h"
#include "Led.h"
#include "Beep.h"
#include "Uart.h"
#include "Timer.h"
#include "FPM_Protocol.h"
#include "ADC.h"
#include "Warning.h"
#include "Task.h"

/* Public variables ----------------------------------------------------------*/
char g_current_mode =NULL;	//V-Verify E-Enroll D-Delete P-Permission C-Connect
char g_previous_mode =NULL;
bool g_bSerialProblem =false;
volatile bool g_UserCancel_Flag =false;

/* Imported variables --------------------------------------------------------*/
extern volatile bool g_bEnroll_Permission_Timeout;

/* Private variables ---------------------------------------------------------*/
static bool bTask_CheckMode =false;
static bool bStopFlag =false;	//user operation finished flag

/* Private function prototypes -----------------------------------------------*/
static void Clock_DCO_Init();
static void Clock_VLO_Init();
static void Stop_UserFinish();
static void SerialProblem();


void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	//Stop Watch dog
	
	Clock_DCO_Init();
#if SleepBatteryCheck
	Clock_VLO_Init();
#endif
	UartInit();
	KeyInit();
	LedInit();
	BeepInit();
	MotorInit();
	FPM_IO_Init();
	ADC_Init();
	TimerInit();
	
	FPM_PowerCmd(ENABLE);
	delayms(FPM_POWER_ON_TIME);
	g_current_mode ='C';	//connect the FPM
	bTask_CheckMode =true;
	
	while(1)
	{
		if(bTask_CheckMode){
			Task_CheckMode();
		}
		if(g_bSerialProblem){
			g_bSerialProblem =false;
			SerialProblem();
		}
		if(bStopFlag){
			bStopFlag =false;
			Stop_UserFinish();
		}
	}
}

/**
 * @brief	PORT2 Interrupt Service Request function
 *			This is used to check the BUTTON and RING
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port_2()
{
	ExternalInterruptCmd(DISABLE);
	Exit_LowPowerMode();
	FPM_PowerCmd(ENABLE);
	TimerB_Cmd(DISABLE);
	if(SET ==GetITStatus(RING)){
		delayms(10);
		if(RING ==KeyScan()){
			delayms(FPM_POWER_ON_TIME);
			g_current_mode ='V';
		}
	}
	else if(SET ==GetITStatus(BUTTON)){
		delayms(10);
		if(g_current_mode ==NULL)
			TimerA_Start(0x01);
		else g_UserCancel_Flag =true;
	}
	else UserStop_Enable();
}


void UserStop_Enable()
{
	bStopFlag =true;
}

static void Stop_UserFinish()
{
	g_current_mode =NULL;
	bTask_CheckMode =true;
	g_UserCancel_Flag =false;
	g_bEnroll_Permission_Timeout =false;
	FPM_PowerCmd(DISABLE);
	BatteryCheck();
	delayms(300);
	ExternalInterruptCmd(ENABLE);
	Goto_Sleep();
}

static void SerialProblem()
{
	Warning_ConnectError();
	UserStop_Enable();
}

/* Clock config --------------------------------------------------------------*/
static void Clock_DCO_Init()
{
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
    DCOCTL = CALDCO_1MHZ;
}

static void Clock_VLO_Init()	//12KHz
{
	//BCSCTL1 &= ~(0x01 <<6);	//XTS =0
	BCSCTL3 |= LFXT1S_2;	//ÉèÖÃLFXT1SxÎª10
	/*
	uint8 OSC_Delay =0;
	do
	{
		IFG1 &= ~OFIFG;
		OSC_Delay =255;
		Watchdog_Clear();
		while(OSC_Delay --);
	}while(IFG1 & OFIFG);*/
}


/* Check the battery voltage -------------------------------------------------*/
void BatteryCheck()
{
	uint16 battery =0;
	
	battery =ADC_Battery_Convert();
	if(battery <BATTERY_WARNING && battery >BATTERY_WARNING -25)
		Warning_LowVoltage();
	if(battery <=BATTERY_WARNING -25)
		Warning_BadVoltage();
}

/* Enter low-power mode ------------------------------------------------------*/
void Goto_Sleep()
{
#if SleepBatteryCheck
	TimerB_Cmd(ENABLE);
#endif
	Enter_LowPowerMode();
}

/* Basic functions -----------------------------------------------------------*/
void delayms(uint16 xms)
{
    uint16 i;
    for(i=xms; i>0; i--)
	{
        __delay_cycles(1000);
    }
}