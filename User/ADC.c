#include "ADC.h"
#include "pin_mcu.h"
#include "time.h"
#include "key.h"

uint8_t ChargingStatus = 0;
uint8_t LED_PWM = 0;
uint8_t ADC_NUM = 0;
uint8_t ADC_Colour = 0;

uint16_t  BatVCount = 0;
uint16_t  BatSum = 0;
uint16_t  BatData = 0;

void ADC_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = BVD_ADC_PIN | USB_ADC_PIN | NTC_ADC_PIN | TEMP_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_DeInit(ADC1);                                            
  ADC_StructInit(&ADC_InitStructure); 

  ADC_GPIO_INIT();

	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b; 
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* 设置 ADC1 使用8转换通道，转换顺序1，采样时间为 55.5 周期 */ 
	//ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);   

	ADC_GetCalibrationFactor(ADC1);         
  ADC_Cmd(ADC1, ENABLE);               
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));
}

uint16_t Get_Adc(unsigned char ch)
{
	ADC_ChannelConfig(ADC1, ch, ADC_SampleTime_239_5Cycles);	

	ADC_StartOfConversion(ADC1);	

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); 

	return ADC_GetConversionValue(ADC1);	
}

void BatVfilter(void)
{
	BatSum+=Get_Adc(ADC_Channel_2);
	BatVCount++;
	if(BatVCount>=16)
	{
			BatVCount=0;
			BatData=(BatSum>>=4);
			BatSum=0;
	}	
}

