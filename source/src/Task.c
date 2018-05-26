#include "Task.h"
#include "FPM_Protocol.h"
#include "Warning.h"
#include "Timer.h"
#include "Motor.h"
#include "Led.h"
#include "Beep.h"

/* Imported variables --------------------------------------------------------*/
extern char g_current_mode;
extern char g_previous_mode;
extern bool g_bSerialProblem;
extern volatile bool g_bEnroll_Permission_Timeout;
extern volatile bool g_UserCancel_Flag;

/* Private variables ---------------------------------------------------------*/
static uint16 FP_Enrolled_Counter =0;
static bool LockFlag =false;
static bool FirstUse_Flag =false;
static uint8 FirstUseFail_Counter =0;

/* Private function prototypes -----------------------------------------------*/
static void Hello_Frankie();
static void LockOpen();
static void LockClose();
static void Demonstration_WithNoFP();
static bool Check_EnrolledCount();
static bool FPM_ConnectInit();
static FP_PermissionRet FP_Permission();

static void Verify_ResultCheck(FP_VerifyRet result);
static void Enroll_ResultCheck(FP_EnrollRet result);
static void Delete_ResultCheck(FP_DeleteRet result);
static void Permission_ResultCheck(FP_PermissionRet result);


/**
 *@brief	Check the current mode, and execute the corresponding operation flow.
 *			
 */
void Task_CheckMode()
{
	static bool bFirstTimePowerOn =true;
	FP_VerifyRet ret1;
	FP_EnrollRet ret2;
	FP_DeleteRet ret3;
	FP_PermissionRet ret4;
	
	switch(g_current_mode)
	{
		case 'V':	//Verify
			if(FP_Enrolled_Counter !=0){
				if(!LockFlag){	//if lock opened
					LockClose();
					UserStop_Enable();
				}
				else{			//if lock closed
					ret1 =FP_Verify();
					Verify_ResultCheck(ret1);
				}
			}
			else{
				Demonstration_WithNoFP();	//No fingerprint enrolled
				g_current_mode ='C';
			}
			break;
		case 'E':	//Enroll
			ret2 =FP_Enroll();
			Enroll_ResultCheck(ret2);
			break;
		case 'D':	//Delete
			ret3 =FP_Delete();
			Delete_ResultCheck(ret3);
			break;
		case 'P':	//Permission
			ret4 =FP_Permission();
			Permission_ResultCheck(ret4);
			break;
		case 'C':	//Connect
			__bis_SR_register(GIE);	//Enable global interrupt
			if(true ==FPM_ConnectInit())
			{
				if(bFirstTimePowerOn){	//if first-time power-on connection
					bFirstTimePowerOn =false;
					Hello_Frankie();
				}
				else 			//if routine serial connection check
					UserStop_Enable();
			}
			else
				g_bSerialProblem =true;
			break;
		default:break;
	}
}

/**
 *@brief	Check the enrolled fingerprints.
 *			If there is only one fingerprint, delete it.
 */
static bool Check_EnrolledCount()
{
	bool ret=FPM_GetEnrolledCount(&FP_Enrolled_Counter, ID_TEMPLATE_BEGIN, ID_TEMPLATE_END);
	if(!ret) return false;
#if FIRST_ENROLL_FPs ==2
	if(FP_Enrolled_Counter ==1)
	{
		if(FP_DELETE_RET_SUCCESS ==FP_Delete())
			FP_Enrolled_Counter =0;
	}
#endif
	return true;
}


/**
 *@brief	Get the permission first if the user wants to enroll another fingerprint
 *			or delete all fingerprints enrolled.
 */
static FP_PermissionRet FP_Permission()
{
	FP_VerifyRet ret;
	
	Check_EnrolledCount();
	if(FP_Enrolled_Counter >0 && FP_Enrolled_Counter <(uint16)ID_TEMPLATE_END)
	{
		while(CheckFinger());
		TimerA_Start(0x0a);
		while(1)	//防止仅触摸ring
		{
			LED_Permission_WaitForFP();
			while(!CheckFinger())
			{
				if(g_UserCancel_Flag)
					return FP_PERMISSION_RET_USER_CANCEL;
				if(g_bEnroll_Permission_Timeout)
					return FP_PERMISSION_RET_TIMEOUT;
			}
			LED_Permission_GetFP();
			ret =FP_Verify();
			if(ret !=FP_VERIFY_RET_GET_NO_IMAGE)
				break;
		}
		if(ret !=FP_VERIFY_RET_SUCCESS)
			return FP_PERMISSION_RET_FAIL;
		else
			return FP_PERMISSION_RET_PASS;
	}
	else if(FP_Enrolled_Counter ==(uint16)ID_TEMPLATE_END)	//Templates reach the maximum
		return FP_PERMISSION_RET_FULL_SPACE;
	else return FP_PERMISSION_RET_DEFAULT_PASS;	//No fingerprint enrolled
}


/* Check Result --------------------------------------------------------------*/
static void Verify_ResultCheck(FP_VerifyRet result)
{
	switch(result)
	{
	case FP_VERIFY_RET_SUCCESS:
		FirstUse_Flag =false;
		//Warning_Verify_Pass();
		LockOpen();
		UserStop_Enable();
		break;
	case FP_VERIFY_RET_FAIL:
#if FP_FirstVerifyCheck
		if((++FirstUseFail_Counter >=5) && FirstUse_Flag){
			FirstUseFail_Counter =0;
			Warning_Verify_FirstUseFail();
			g_current_mode ='D';
		}
		else{
			Warning_Verify_Fail();
			UserStop_Enable();
		}
#else
		Warning_Verify_Fail();
		UserStop_Enable();
#endif
		break;
	case FP_VERIFY_RET_GET_NO_IMAGE:
		Warning_Verify_RingTouch();
		UserStop_Enable();
		break;
	}
}

static void Enroll_ResultCheck(FP_EnrollRet result)
{
	static uint8 MergeFail_Counter =0;
	
	switch(result)
	{
	case FP_ENROLL_RET_FAIL:
	case FP_ENROLL_RET_STORE_FAIL:
	case FP_ENROLL_RET_NO_VALID_ID:
		Warning_Enroll_Fail();
		MergeFail_Counter =0;
		UserStop_Enable();
		break;
	case FP_ENROLL_RET_MERGE_FAIL:
		Warning_Enroll_MergeFail();
		if(++MergeFail_Counter ==3){
			MergeFail_Counter =0;
#if FIRST_ENROLL_FPs ==2
			if(FP_Enrolled_Counter ==1)	//如果已录入一个指纹
				g_current_mode ='D';
			else
#endif
				UserStop_Enable();
		}
		else
			g_current_mode ='E';
		break;
	case FP_ENROLL_RET_SUCCESS:
		Warning_Enroll_Success();
		MergeFail_Counter =0;
		FP_Enrolled_Counter ++;
#if FIRST_ENROLL_FPs ==2
		if(FP_Enrolled_Counter ==1)	//enroll another one if there is only 1 fingerprint enrolled
			g_current_mode ='E';
		else
#endif
			UserStop_Enable();
		break;
	case FP_ENROLL_RET_TIMEOUT:
		Warning_Enroll_TimeOut();
		MergeFail_Counter =0;
		Check_EnrolledCount();
		UserStop_Enable();
		break;
	case FP_ENROLL_RET_USER_CANCEL:
		LED_UserCancel();
		MergeFail_Counter =0;
		UserStop_Enable();
		break;
	default:break;
	}
}

static void Delete_ResultCheck(FP_DeleteRet result)
{
	switch(result)
	{
	case FP_DELETE_RET_FAIL:
		Warning_Delete_Fail();
		UserStop_Enable();
		break;
	case FP_DELETE_RET_SUCCESS:
		FP_Enrolled_Counter =0;
		FirstUse_Flag =false;
		FirstUseFail_Counter =0;
		Warning_Delete_Success();
		UserStop_Enable();
		break;
	default:break;
	}
}

static void Permission_ResultCheck(FP_PermissionRet result)
{
	static uint8 PermissionFail_Counter =0;
	
	switch(result)
	{
	case FP_PERMISSION_RET_FAIL:
		Warning_Permission_Fail();
		if(++PermissionFail_Counter ==PERMISSION_CHANCES){
			PermissionFail_Counter =0;
			UserStop_Enable();
		}
		else
			g_current_mode ='P';
		break;
	case FP_PERMISSION_RET_PASS:	//previous mode?--'E' or 'D'
		Warning_Permission_Pass();
		PermissionFail_Counter =0;
#if	ContinualEnroll_MODE
		while(CheckFinger());
#endif
		if(g_previous_mode =='E')
			g_current_mode ='E';
		else if(g_previous_mode =='D')
			g_current_mode ='D';
		break;
	case FP_PERMISSION_RET_TIMEOUT:
		Warning_Permission_TimeOut();
		PermissionFail_Counter =0;
		UserStop_Enable();
		break;
	case FP_PERMISSION_RET_USER_CANCEL:
		LED_UserCancel();
		PermissionFail_Counter =0;
		UserStop_Enable();
		break;
	case FP_PERMISSION_RET_DEFAULT_PASS:	//No fingerprint enrolled
		if(g_previous_mode =='E'){
#if FP_FirstVerifyCheck
			FirstUse_Flag =true;
#endif
			g_current_mode ='E';
		}
		else if(g_previous_mode =='D'){
			Warning_Delete_Fail();
			UserStop_Enable();
		}
		break;
	case FP_PERMISSION_RET_FULL_SPACE:
		Warning_Enroll_FullSpace();
		UserStop_Enable();
		break;
	default:break;
	}
}

/**
 *@brief	Say hello after the system successfully start.
 *			
 */
static void Hello_Frankie()
{
	Led_On(LED_BLUE);
	Led_Off(LED_RED);
	for(uint8 i =0; i<6; i++)
	{
		delayms(100);
		Led_Toggle(LED_BLUE);
		Led_Toggle(LED_RED);
	}
	delayms(100);
	
	Led_On(LED_BLUE);
	Led_On(LED_RED);
	delayms(1000);
	LockOpen();
	
	Led_On(LED_BLUE);
	Led_On(LED_RED);
	delayms(1000);
	LockClose();
	
	delayms(200);
	for(uint8 i =0; i<4; i++)
	{
		Led_Toggle(LED_BLUE);
		delayms(150);
	}
	Led_Off(LED_BLUE);
	Led_Off(LED_RED);
}

/**
 *@brief	Connect the FPM, set some parameters and check enrolled fingerprint.
 *			If there is only one fingerprint, delete it.
 */
static bool FPM_ConnectInit()
{
	bool ret =false;
	
	ret =FPM_TestConnection();
	if(!ret) return false;
	ret =FPM_SetParameter(PARAM_TYPE_DUPLICATION_CHECK, 0);
	if(!ret) return false;
	ret =FPM_SetParameter(PARAM_TYPE_AUTO_LEARN, 1);
	if(!ret) return false;
	ret =Check_EnrolledCount();
	if(!ret) return false;
	return true;
}


/**
 *@brief	If there is no fingerprint existed, the lock works as a demo.
 *
 */
static void Demonstration_WithNoFP()
{
	if(LockFlag) LockOpen();
	else LockClose();
}

/**
 *@brief	Open the lock, with blue LED on.
 *
 */
static void LockOpen()
{
	Led_On(LED_BLUE);
	Led_Off(LED_RED);
	
	LockFlag =false;
	MotorForward();
	MotorKeep();
	uint16 time =Warning_LockOpen();
	Led_On(LED_BLUE);
	delayms(MOTOR_RUN_TIME -time);
	MotorStandby();
	
	Led_Off(LED_BLUE);
}

/**
 *@brief	Close the lock, with red LED on.
 *
 */
static void LockClose()
{
	Led_On(LED_RED);
	Led_Off(LED_BLUE);
	
	LockFlag =true;
	MotorReverse();
	MotorKeep();
	uint16 time =Warning_LockClose();
	Led_On(LED_RED);
	delayms(MOTOR_RUN_TIME -time);
	MotorStandby();
	
	Led_Off(LED_RED);
}

