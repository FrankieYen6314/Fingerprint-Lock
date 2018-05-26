#ifndef _TASK_H_
#define _TASK_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"


typedef enum
{
	FP_PERMISSION_RET_FAIL=0,
	FP_PERMISSION_RET_PASS,
	FP_PERMISSION_RET_TIMEOUT,
	FP_PERMISSION_RET_USER_CANCEL,
	FP_PERMISSION_RET_DEFAULT_PASS,
	FP_PERMISSION_RET_FULL_SPACE
}FP_PermissionRet;

/* Exported Functions --------------------------------------------------------*/
void Task_CheckMode();


#endif