#ifndef _WARNING_H_
#define _WARNING_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"
#include "Config.h"

/* Exported Functions --------------------------------------------------------*/
uint16 Warning_ConnectError();

uint16 Warning_Verify_Pass();
uint16 Warning_Verify_FirstUseFail();
uint16 Warning_Verify_Fail();
uint16 Warning_Verify_RingTouch();

uint16 Warning_Enroll_Start();
uint16 Warning_Enroll_Success();
uint16 Warning_Enroll_Fail();
uint16 Warning_Enroll_BadImage();
uint16 Warning_Enroll_QualifiedImage();
uint16 Warning_Enroll_MergeFail();
uint16 Warning_Enroll_TimeOut();
uint16 Warning_Enroll_FullSpace();

uint16 Warning_Delete_Start();
uint16 Warning_Delete_Fail();
uint16 Warning_Delete_Success();

uint16 Warning_Permission_Fail();
uint16 Warning_Permission_Pass();
uint16 Warning_Permission_TimeOut();

uint16 Warning_LockOpen();
uint16 Warning_LockClose();

uint16 Warning_LowVoltage();
uint16 Warning_BadVoltage();

void LED_UserCancel();
void LED_Enroll_WaitForFP();
void LED_Enroll_GetFP();
void LED_Permission_WaitForFP();
void LED_Permission_GetFP();

#endif