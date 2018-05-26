#ifndef _FPM_PROTOCL_H_
#define _FPM_PROTOCL_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

/* Command list --------------------------------------------------------------*/
#define	CMD_TEST_CONNECTION		0x0001
#define	CMD_SET_PARAM			0x0002
#define	CMD_GET_PARAM			0x0003
#define	CMD_GET_DEVICE_INFO		0x0004
#define	CMD_GET_IMAGE			0x0020
#define	CMD_STORE_CHAR			0x0040
#define	CMD_LOAD_CHAR			0x0041
#define	CMD_DEL_CHAR			0x0044
#define	CMD_GET_EMPTY_ID		0x0045
#define	CMD_GET_STATUS			0x0046
#define	CMD_GET_ENROLL_COUNT	0x0048
#define	CMD_GENERATE			0x0060
#define	CMD_MERGE				0x0061
#define	CMD_MATCH				0x0062
#define	CMD_SEARCH				0x0063
#define	CMD_VERIFY				0x0064

/* Parameter Type ------------------------------------------------------------*/
#define PARAM_TYPE_DEVICE_ID			0
#define PARAM_TYPE_SECURITY_LEVEL		1
#define PARAM_TYPE_DUPLICATION_CHECK	2
#define PARAM_TYPE_BAUDRATE				3
#define PARAM_TYPE_AUTO_LEARN			4

/* Response and error code ---------------------------------------------------*/
#define ERR_SUCCESS				0x00
#define ERR_FAIL				0x01
#define ERR_VERIFY				0x10
#define ERR_IDENTIFY			0x11
#define ERR_TMPL_EMPTY			0x12
#define ERR_TMPL_NOT_EMPTY		0x13
#define ERR_ALL_TMPL_EMPTY		0x14
#define ERR_EMPTY_ID_NOEXIST	0x15
#define ERR_INVALID_TMPL_DATA	0x17
#define ERR_DUPLICATION_ID		0x18
#define ERR_BAD_QUALITY			0x19
#define ERR_MERGE_FAIL			0x1A
#define ERR_INVALID_TMPL_NO		0x1D
#define ERR_INVALID_PARAM		0x22
#define ERR_GEN_COUNT			0x25
#define ERR_INVALID_BUFFER_ID	0x26
#define ERR_FP_NOT_DETECTED		0x28


#define	COMMAND_PREFIX_VALUE	0xAA55
#define	RESPONSE_PREFIX_VALUE	0x55AA
#define PACKAGE_CKS_SIZE		2		//Check Sum

#define ID_TEMPLATE_BEGIN	1
#define	ID_TEMPLATE_END		100
#define	ID_MERGE_RAMBUFFER	0


typedef struct
{
	uint16	prefix;
	uint8	sid;
	uint8	did;
	uint16	cmd;
	uint16	len;
	uint8	data[16];
	uint16	cks;
}CommandPackage;

typedef struct
{
	uint16	prefix;
	uint8	sid;
	uint8	did;
	uint16	rcm;
	uint16	len;
	uint16	ret;
	uint8	data[14];
	uint16	cks;
}ResponsePackage;

typedef union
{
	CommandPackage	command;
	ResponsePackage	response;
}PackageUnion;

typedef enum
{
	FP_VERIFY_RET_FAIL =0,
	FP_VERIFY_RET_SUCCESS,
	FP_VERIFY_RET_GET_NO_IMAGE
}FP_VerifyRet;

typedef enum
{
	FP_ENROLL_RET_FAIL =0,
	FP_ENROLL_RET_SUCCESS,
	FP_ENROLL_RET_TIMEOUT,
	FP_ENROLL_RET_USER_CANCEL,
	FP_ENROLL_RET_NO_VALID_ID,
	FP_ENROLL_RET_MERGE_FAIL,
	FP_ENROLL_RET_STORE_FAIL
}FP_EnrollRet;

typedef enum
{
	FP_DELETE_RET_FAIL =0,
	FP_DELETE_RET_SUCCESS
}FP_DeleteRet;


/* Exported Functions --------------------------------------------------------*/
void FPM_IO_Init();
void FPM_PowerCmd(FunctionalState NewState);
uint16 CheckSum(PackageUnion *package);
void SendCommand(uint8 *Data, uint8 u8Length);
bool ReceiveCommand(uint8 *Data, uint8 u8Length);

bool CheckFinger();

bool FPM_TestConnection();
bool FPM_SetParameter(uint8 ParamType, uint8 ParamValue);
bool FPM_CaptureFingerImage();
bool FPM_GenerateTemplate(uint16 RamBuffer_ID);
bool FPM_MergeTemplate(uint16 dst_RamBuffer_ID, uint8 MergeCount);
bool FPM_GetValidTemplateID(uint16 *Get_Template_ID, uint16 Begin_Template_ID, uint16 End_Template_ID);
bool FPM_StoreTemplate(uint16 Store_Template_ID, uint16 Source_RamBuffer_ID);
bool FPM_SearchTemplate(uint16 RamBuffer_ID, uint16 Begin_Template_ID, uint16 End_Template_ID);
bool FPM_DeleteAllTemplates(uint16 Begin_Template_ID, uint16 End_Template_ID);
bool FPM_GetEnrolledCount(uint16 *enrolled_count, uint16 Begin_Template_ID, uint16 End_Template_ID);

FP_VerifyRet FP_Verify();
FP_EnrollRet FP_Enroll();
FP_DeleteRet FP_Delete();

#endif