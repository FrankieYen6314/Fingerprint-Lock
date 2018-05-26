#include "FPM_Protocol.h"
#include "config.h"
#include "Warning.h"
#include "Uart.h"
#include "Key.h"
#include "Timer.h"
#include <string.h>

/* Imported variables --------------------------------------------------------*/
extern volatile bool g_bUartFlag;
extern volatile bool g_bEnroll_Permission_Timeout;
extern volatile bool g_UserCancel_Flag;
extern bool g_bSerialProblem;
extern uint8 UartBuffer[26];

/* Private variables ----------------------------------------------------------*/
static uint8 Buffer_LastData[26];

/* Private function prototypes -----------------------------------------------*/
static bool CheckResponseProblem(uint16 xms);


void FPM_IO_Init()
{
	P1DIR |= PW_CTR;    //P1.5 output mode, control the power of the FPM
    P1OUT &= ~PW_CTR;   //Turn FVCC off
}

/**
  *@brief	Enables or disables the FPM power.
  *@param	NewState: specifies the new state of FPM power.
  *			This parameter can be one of ENABLE or DISABLE.
  *@retval	None
  */
void FPM_PowerCmd(FunctionalState NewState)
{
	if(NewState !=DISABLE)
	{
		P1OUT |= PW_CTR;
	}
	else
	{
		P1OUT &= ~PW_CTR;
	}
}

/**
  *@brief	Check the low word of data package from PREFIX to DATA, 
  *			calculate the arithmetic sum.
  *@param	package: pointer to PackageUnion.
  *@retval	the arithmetic sum
  */
uint16 CheckSum(PackageUnion *package)
{
	uint16 sum =0;
	uint8 *buffer =(uint8*)package;
	
	for(uint8 i =0; i<(sizeof(PackageUnion)-PACKAGE_CKS_SIZE); i++)
	{
		sum +=buffer[i];
	}
	return sum;
}

/**
  *@brief	Send the command by serial interface.
  *@param	Data: pointer to type uint8(unsigned char).
  *@param	u8Length: specifies the length of the command.
  *@retval	NONE
  */
void SendCommand(uint8 *Data, uint8 u8Length)
{
	g_bUartFlag =false;
	memcpy(Buffer_LastData, Data, u8Length);
	while(u8Length--)
	{
		SerialPutChar(*Data);
		Data++;
	}
}

/**
  *@brief	Receive the command by serial interrupt.
  *@param	Data: pointer to type uint8(unsigned char).
  *@param	u8Length: specifies the length of the command.
  *@retval	false: failed to receive response data
  * 		true: succeeed in receiving response data
  */
bool ReceiveCommand(uint8 *Data, uint8 u8Length)
{
	static uint16 UartCounter =0;
	static uint8 ResponseFail_Counter =0;
	bool ret =false;
	
	while(!g_bUartFlag)	//Wait for response command
	{
		delayms(1);
		UartCounter ++;
		ret =CheckResponseProblem(UartCounter);
		if(ret)
		{
			UartCounter =0;
			if(++ResponseFail_Counter ==3)
			{
				ResponseFail_Counter =0;
				g_bSerialProblem =true;
				return false;
			}
		}
	}
	UartCounter =0;
	ResponseFail_Counter =0;
	memcpy(Data, UartBuffer, u8Length);
	return true;
}

static bool CheckResponseProblem(uint16 xms)
{
	if(xms >UART_ERROR_TIME)
	{
		SendCommand(Buffer_LastData, 26);
		return true;
	}
	else return false;
}

bool CheckFinger()
{
	if(RING ==KeyScan())
		return true;
	else
		return false;
}


bool FPM_TestConnection()
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_TEST_CONNECTION;
	package.command.len =0;
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_TEST_CONNECTION)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_SetParameter(uint8 ParamType, uint8 ParamValue)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_SET_PARAM;
	package.command.len =5;
	package.command.data[0] =ParamType;
	package.command.data[1] =ParamValue;
	//memcpy(&package.command.data[1], &ParamValue, 1);
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_SET_PARAM)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_CaptureFingerImage()
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_GET_IMAGE;
	package.command.len =0;
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_GET_IMAGE)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_GenerateTemplate(uint16 RamBuffer_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_GENERATE;
	package.command.len =2;
	memcpy(&package.command.data[0], &RamBuffer_ID, sizeof(RamBuffer_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_GENERATE)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_MergeTemplate(uint16 dst_RamBuffer_ID, uint8 MergeCount)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_MERGE;
	package.command.len =3;
	memcpy(&package.command.data[0], &dst_RamBuffer_ID, sizeof(dst_RamBuffer_ID));
	memcpy(&package.command.data[2], &MergeCount, sizeof(MergeCount));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_MERGE)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_GetValidTemplateID(uint16 *Get_Template_ID, uint16 Begin_Template_ID, uint16 End_Template_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_GET_EMPTY_ID;
	package.command.len =4;
	memcpy(&package.command.data[0], &Begin_Template_ID, sizeof(Begin_Template_ID));
	memcpy(&package.command.data[2], &End_Template_ID, sizeof(End_Template_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_GET_EMPTY_ID)
	  &&(package.response.len ==(sizeof(package.response.ret)+2))
	  &&(package.response.ret ==ERR_SUCCESS))
	{
		memcpy(Get_Template_ID, &package.response.data[0], 2);
		return true;
	}
	else
		return false;
}

bool FPM_StoreTemplate(uint16 Store_Template_ID, uint16 Source_RamBuffer_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_STORE_CHAR;
	package.command.len =4;
	memcpy(&package.command.data[0], &Store_Template_ID, sizeof(Store_Template_ID));
	memcpy(&package.command.data[2], &Source_RamBuffer_ID, sizeof(Source_RamBuffer_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_STORE_CHAR)
	  &&(((package.response.ret ==ERR_SUCCESS)&&(package.response.len ==sizeof(package.response.ret)))
	  ||((package.response.ret ==ERR_DUPLICATION_ID)&&(package.response.len ==sizeof(package.response.ret) +2))))
		return true;
	else
		return false;
}

bool FPM_SearchTemplate(uint16 RamBuffer_ID, uint16 Begin_Template_ID, uint16 End_Template_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_SEARCH;
	package.command.len =6;
	memcpy(&package.command.data[0], &RamBuffer_ID, sizeof(RamBuffer_ID));
	memcpy(&package.command.data[2], &Begin_Template_ID, sizeof(Begin_Template_ID));
	memcpy(&package.command.data[4], &End_Template_ID, sizeof(End_Template_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_SEARCH)
	  &&(package.response.len ==(sizeof(package.response.ret)+3))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_DeleteAllTemplates(uint16 Begin_Template_ID, uint16 End_Template_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_DEL_CHAR;
	package.command.len =4;
	memcpy(&package.command.data[0], &Begin_Template_ID, sizeof(Begin_Template_ID));
	memcpy(&package.command.data[2], &End_Template_ID, sizeof(End_Template_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_DEL_CHAR)
	  &&(package.response.len ==sizeof(package.response.ret))
	  &&(package.response.ret ==ERR_SUCCESS))
		return true;
	else
		return false;
}

bool FPM_GetEnrolledCount(uint16 *enrolled_count, uint16 Begin_Template_ID, uint16 End_Template_ID)
{
	PackageUnion package;
	memset((char*)&package, 0, sizeof(PackageUnion));
	
	package.command.prefix =COMMAND_PREFIX_VALUE;
	package.command.cmd =CMD_GET_ENROLL_COUNT;
	package.command.len =4;
	memcpy(&package.command.data[0], &Begin_Template_ID, sizeof(Begin_Template_ID));
	memcpy(&package.command.data[2], &End_Template_ID, sizeof(End_Template_ID));
	package.command.cks =CheckSum(&package);
	SendCommand((uint8 *)&package, sizeof(PackageUnion));
	
	bool ret =ReceiveCommand((uint8 *)&package, sizeof(PackageUnion));
	if(!ret) return false;
	uint16 response_cks =CheckSum(&package);
	if ((package.response.cks ==response_cks)
	  &&(package.response.prefix ==RESPONSE_PREFIX_VALUE)
	  &&(package.response.rcm ==CMD_GET_ENROLL_COUNT)
	  &&(package.response.len ==(sizeof(package.response.ret) +2))
	  &&(package.response.ret ==ERR_SUCCESS))
	{
		memcpy(enrolled_count, &package.response.data[0], 2);
		return true;
	}
	else
		return false;
}


FP_VerifyRet FP_Verify()
{
	bool ret =false;
	
	ret =FPM_CaptureFingerImage();
	if(!ret) return FP_VERIFY_RET_GET_NO_IMAGE;
	ret =FPM_GenerateTemplate(ID_MERGE_RAMBUFFER);
	if(!ret) return FP_VERIFY_RET_FAIL;
	ret =FPM_SearchTemplate(ID_MERGE_RAMBUFFER, ID_TEMPLATE_BEGIN, ID_TEMPLATE_END);
	if(!ret) return FP_VERIFY_RET_FAIL;
	else return FP_VERIFY_RET_SUCCESS;
}

FP_EnrollRet FP_Enroll()
{
	bool ret =false;
	uint8 EnrollFail_Counter =0;
	uint16 get_template_id =0;
	
	ret =FPM_GetValidTemplateID(&get_template_id, ID_TEMPLATE_BEGIN, ID_TEMPLATE_END);
	if(!ret) return FP_ENROLL_RET_NO_VALID_ID;
	for(uint16 enroll_step=0; enroll_step<FP_MERGE_COUNT; enroll_step++)
	{
		TimerA_Start(0x06);
		while(1)
		{
			LED_Enroll_WaitForFP();
			while(!CheckFinger())
			{
				if(g_UserCancel_Flag)
					return FP_ENROLL_RET_USER_CANCEL;
				if(g_bEnroll_Permission_Timeout)
					return FP_ENROLL_RET_TIMEOUT;
			}
			LED_Enroll_GetFP();
			ret =FPM_CaptureFingerImage();	//if no finger image captured, then check the finger again
			if(!ret) continue;
			ret =FPM_GenerateTemplate(enroll_step);
			if(!ret){
				Warning_Enroll_BadImage();
#if	ContinualEnroll_MODE
				delayms(500);
#else
				while(CheckFinger());
#endif
				if(++EnrollFail_Counter >=5)
					return FP_ENROLL_RET_FAIL;
				continue;
			}
			Warning_Enroll_QualifiedImage();
#if	ContinualEnroll_MODE
			delayms(500);
#else
			while(CheckFinger());
#endif
			break;
		}
	}
	ret =FPM_MergeTemplate(ID_MERGE_RAMBUFFER, FP_MERGE_COUNT);
	if(!ret) return FP_ENROLL_RET_MERGE_FAIL;
	ret =FPM_StoreTemplate(get_template_id, ID_MERGE_RAMBUFFER);
	if(!ret) return FP_ENROLL_RET_STORE_FAIL;
	return FP_ENROLL_RET_SUCCESS;
}

FP_DeleteRet FP_Delete()
{
	bool ret =false;
	
	ret =FPM_DeleteAllTemplates(ID_TEMPLATE_BEGIN, ID_TEMPLATE_END);
	if(!ret) return FP_DELETE_RET_FAIL;
	return FP_DELETE_RET_SUCCESS;
}