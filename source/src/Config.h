#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <msp430g2433.h>
#include "MacroAndConst.h"

#define	E_BTN		BIT1	//Enroll Button	(P2.1)
#define	V_BTN		BIT2	//Verify Button	(P2.2)
#define	SPK			BIT3	//Buzzer Control(P2.3)
#define	LED_R		BIT0	//RED LED   (P2.0)
#define	LED_B		BIT6	//BLUE LED  (P2.6)
#define	PW_CTR		BIT5	//FVCC Power Control(P1.5)
#define	MOT_INA		BIT4	//Motor Control INA	(P2.4)
#define	MOT_INB		BIT5	//Motor Control INB (P2.5)
#define LimitSW		BIT0	//Limit Switch	(P1.0)
#define	ADC_EN		BIT4	//ADC Enable	(P1.4)

//Preprocessor
#define SleepBatteryCheck		1		//˯��״̬��ص�����ʱ���(1:���, 0:�����)
#define FP_FirstVerifyCheck		0		//�״�¼��ָ�ƺ�ѡ���Ƿ����״αȶ�ʱ�����¼��ָ�Ƶ���Ч��
#define ContinualEnroll_MODE	1		//1:����¼��ģʽ��0��¼��ʱ��̧����ָ
#define FIRST_ENROLL_FPs		1		//�״�¼��ָ����(1��2)
//#define USE_LIMIT_SW			0


//Macro Definitions
#define FP_MERGE_COUNT		3		//ָ�ƻ����3(2��3)
#define PERMISSION_CHANCES	3		//Ȩ��ȷ�ϴ������3��
#define MOTOR_RUN_TIME		120		//���ת��ʱ��ms(����С��120ms)(������200ms)
#define LONG_PRESS_TIME		2000	//����ʱ������ms(10ms�ж�һ��)
#define FPM_POWER_ON_TIME	190		//ָ��ģ���ϵ��ʼ���ȴ�ʱ��ms
#define UART_ERROR_TIME		400		//Uartͨ�Ŵ�����ʱ��ms
#define ENROLL_PERMI_TIME	10000	//ָ��¼��ʱ���Ȩ��ȷ��ʱ��10s
#define LED_FLASH_TIME		20		//LED��˸��ƽ��ת����20ms
#define BATTERY_TEST_HOUR	5		//check the battery voltage every 5 hours
#define BATTERY_WARNING		610		//Լ3.61V�����������ֵ(582--3.45V,600--3.55V,616--3.65V,650--3.85V)


#if	SleepBatteryCheck	//Enter Low Power Mode 3(LPM3) w/interrupt, LPM3 Disables CPU, only ACLK is active
#define	Enter_LowPowerMode()		(__bis_SR_register(LPM3_bits + GIE))
#define Exit_LowPowerMode()			(LPM3_EXIT)
#else					//Enter Low Power Mode 4(LPM4), can only be waked up by external interrupt
#define	Enter_LowPowerMode()		(__bis_SR_register(LPM4_bits + GIE))
#define	Exit_LowPowerMode()			(LPM4_EXIT)
#endif


typedef enum {RESET =0, SET =!RESET} FlagStatus;

typedef enum
{
	DISABLE	=0,
	ENABLE	=!DISABLE
}FunctionalState;

/* Exported Functions --------------------------------------------------------*/
void UserStop_Enable();
void BatteryCheck();
void Goto_Sleep();
void delayms(uint16 xms);

#endif