#include "ADC.h"

/* Private defines -----------------------------------------------------------*/
#define	ADC_TEST_NUM	3

/* Private function prototype ------------------------------------------------*/
static void ADC_Start();
static void ADC_Stop();


void ADC_Init()
{
	P1DIR |= ADC_EN;

    ADC10CTL1 |= CONSEQ_2;	//��ͨ���ظ�����ģʽ
    ADC10CTL0 |= SREF_1+ REFON + REF2_5V +REFBURST;
			//ѡ���ڲ��ο�Դ2.5V���򿪻�׼Դ������Ҫ����ת��ʱ�ر��ڲ��ο�Դ
    ADC10CTL0 |= ADC10SHT_2 + MSC;
			//��������16 x ADC10CLK
    ADC10CTL1 |= ADC10SSEL_3 +ADC10DIV_7 +SHS_0;
			//SMCLK8��ƵΪ����ʱ��,��ADC10SC�����ɼ�
    ADC10CTL1 |= INCH_3;	//ѡ��ͨ��A3
    ADC10CTL0 |= ADC10ON;	//����ADC10
    ADC10AE0 |= BIT3;		//�����ⲿͨ��A3
	
	ADC_Stop();
}


static void ADC_Start()
{
	ADC10CTL0 |= ADC10ON + REFON;	//����ADC10�����ڲ��ο�Դ
	ADC10AE0 |= BIT3;	//�����ⲿͨ��A3
	
	P1OUT |= ADC_EN;
	delayms(50);		//����ADCʹ�ܽź󣬵ȴ�50ms��Ӳ��׼��
}

static void ADC_Stop()
{
	ADC10CTL0 &= ~(ENC + ADC10SC);
	P1OUT &= ~ADC_EN;
	
	ADC10CTL0 &= ~(ADC10ON + REFON);	//�ر�ADC10���ر��ڲ��ο�Դ
	ADC10AE0 &= ~BIT3;	//�ر��ⲿͨ��A3
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
				//�ȴ�ADC10IFG��־��ߣ�ת����ɣ�
		ADC10_Result += ADC10MEM;
		delayms(1);	//Set some time for next convertion, or it'll be too quick to detect the voltage
	}
	ADC_Stop();
	
	return (ADC10_Result/ADC_TEST_NUM);
}