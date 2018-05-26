#include "ADC.h"

/* Private defines -----------------------------------------------------------*/
#define	ADC_TEST_NUM	3

/* Private function prototype ------------------------------------------------*/
static void ADC_Start();
static void ADC_Stop();


void ADC_Init()
{
	P1DIR |= ADC_EN;

    ADC10CTL1 |= CONSEQ_2;	//单通道重复采样模式
    ADC10CTL0 |= SREF_1+ REFON + REF2_5V +REFBURST;
			//选择内部参考源2.5V，打开基准源，不需要采样转换时关闭内部参考源
    ADC10CTL0 |= ADC10SHT_2 + MSC;
			//采样周期16 x ADC10CLK
    ADC10CTL1 |= ADC10SSEL_3 +ADC10DIV_7 +SHS_0;
			//SMCLK8分频为采样时钟,用ADC10SC触发采集
    ADC10CTL1 |= INCH_3;	//选择通道A3
    ADC10CTL0 |= ADC10ON;	//开启ADC10
    ADC10AE0 |= BIT3;		//开启外部通道A3
	
	ADC_Stop();
}


static void ADC_Start()
{
	ADC10CTL0 |= ADC10ON + REFON;	//开启ADC10，打开内部参考源
	ADC10AE0 |= BIT3;	//开启外部通道A3
	
	P1OUT |= ADC_EN;
	delayms(50);		//开启ADC使能脚后，等待50ms的硬件准备
}

static void ADC_Stop()
{
	ADC10CTL0 &= ~(ENC + ADC10SC);
	P1OUT &= ~ADC_EN;
	
	ADC10CTL0 &= ~(ADC10ON + REFON);	//关闭ADC10，关闭内部参考源
	ADC10AE0 &= ~BIT3;	//关闭外部通道A3
}

uint16 ADC_Battery_Convert()
{
	uint16 ADC10_Result =0;
	
	ADC_Start();
	for(uint8 i =0; i<ADC_TEST_NUM; i++)
	{
		ADC10CTL0 |= ENC + ADC10SC;
				//ADC10 enabled__Start sample-and-conversion,ADC10SC is reset automatically
		while((ADC10CTL0 & ADC10IFG) ==0);
				//等待ADC10IFG标志变高（转换完成）
		ADC10_Result += ADC10MEM;
		delayms(1);	//Set some time for next convertion, or it'll be too quick to detect the voltage
	}
	ADC_Stop();
	
	return (ADC10_Result/ADC_TEST_NUM);
}