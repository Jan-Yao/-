#include "GPIO.h"
#include "Sensor_I2C.h"
#include "pin_mcu.h"
#include "Charging.h"

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	
	/*LED双色灯 | 脉冲灯*/
  GPIO_uInitStructure.GPIO_Pin =  LED_PWR_G_PIN | LED_PWR_R_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_OD; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
	GPIO_SetBits(LED_PWR_G_GBIO,LED_PWR_G_PIN);
	GPIO_SetBits(LED_PWR_R_GBIO,LED_PWR_R_PIN);
		
	/*脉冲按键*/
	GPIO_uInitStructure.GPIO_Pin = KEY_PULSE_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_uInitStructure);

	/*电源按键*/
	GPIO_uInitStructure.GPIO_Pin = KEY_P_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;   
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	/*LOCK*/
	GPIO_uInitStructure.GPIO_Pin = LOCK_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;   
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	/*NTC_EN*/
	GPIO_uInitStructure.GPIO_Pin = NTC_EN_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	GPIO_SetBits(NTC_EN_GBIO,NTC_EN_PIN);
	
	/*TEMP_EN*/
	GPIO_uInitStructure.GPIO_Pin = TEMP_EN_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
	GPIO_SetBits(TEMP_EN_GBIO,TEMP_EN_PIN);
}

void EXIT_Init(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
 
	/* Configure Button pin as input */
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//  GPIO_InitStructure.GPIO_Pin = LOCK_PIN;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
//	EXTI_InitStruct.EXTI_Line = EXTI_Line12;
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStruct); 
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
	EXTI_InitStruct.EXTI_Line = EXTI_Line13;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); 

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Sleep_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF,ENABLE);
	
	//GPIOA
	GPIO_uInitStructure.GPIO_Pin = USB_ADC_PIN | BVD_ADC_PIN | NTC_ADC_PIN | TEMP_ADC_PIN | KEY_PULSE_PIN | V_CHECK_PIN | H_CHECK_PIN | LD_H_PIN | LD_V_PIN
	                               | D_TX_PIN | D_RX_PIN | Accelerometer_SDA_PIN | Accelerometer_SCL_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
	
	//GPIOB
	GPIO_uInitStructure.GPIO_Pin =  LED_HV_PIN | Charging_SCL_PIN | Charging_SDA_PIN | LED_H_PIN | LED_V_PIN | USART_TX_PIN | USART_RX_PIN | CH_EN_PIN | BVC_PIN
	                                | LOCK_PIN | NTC_EN_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	//GPIOC
	GPIO_uInitStructure.GPIO_Pin =  TEMP_EN_PIN | LED_PWR_G_PIN | LED_PWR_R_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
}

void Sleep_Mode(void)
{
//	EnterSleepProcess();     //关充电
	Accelerometer_Sleep();   //关加速度传感器
	GPIO_SetBits(NTC_EN_GBIO,NTC_EN_PIN);
	GPIO_SetBits(TEMP_EN_GBIO,TEMP_EN_PIN);   //没有下拉电阻时需置位
	ADC_Cmd(ADC1, DISABLE);	        				
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);	
	ADC_DeInit(ADC1);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);        //使能中断源
	TIM_Cmd(TIM3, DISABLE);    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);  
	
	TIM_Cmd(TIM1, DISABLE);    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,DISABLE);  
	
	EXIT_Init();
	Sleep_IO_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
}
