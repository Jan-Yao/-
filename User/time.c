#include "time.h"
#include "key.h"
#include "pin_mcu.h"
#include "LD.h"
#include "ADC.h"
#include "common.h"

/*********************************************************************/

void LED_PWM_OFF(void)
{
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable); //关LED_PWM
}

void LED_H_PWM_ON(void)
{
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable); 
}

void LED_V_PWM_ON(void)
{
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable); 
}

void LED_HV_PWM_ON(void)
{
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable); 
}

void SET_TIM3_PWM_NUM(uint16_t Compare1)
{
	TIM_SetCompare1(TIM3,Compare1);
	TIM_SetCompare2(TIM3,Compare1);
	TIM_SetCompare3(TIM3,Compare1);
}

uint16_t RED_TIM3_PWM_NUM(void)
{
	uint16_t TIM3_PWM_NUM_1 = 0;
	uint16_t TIM3_PWM_NUM_2 = 0;
	uint16_t TIM3_PWM_NUM_3 = 0;
	
	TIM3_PWM_NUM_1 = TIM3->CCR1;
	TIM3_PWM_NUM_2 = TIM3->CCR2;
	TIM3_PWM_NUM_3 = TIM3->CCR3;
	
	if((TIM3_PWM_NUM_1 == TIM3_PWM_NUM_2) && (TIM3_PWM_NUM_2 == TIM3_PWM_NUM_3))
	{
		return TIM3_PWM_NUM_1;
	}
	else
	{
		return 0;
	}
}

/*********************************************************************/

void PWM_LED_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LED_HV_PIN | LED_H_PIN | LED_V_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_1);
}

/*********************************************************************
*功能：定时器初始化
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void TIME_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //定义tim结构体变量
	TIM_OCInitTypeDef  TIM_OCInitStructure;         //PWM结构体
	
	PWM_LED_GPIO_INIT();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //使能tim2  TIM2被隐藏，规格书中没有，但是能用
	
  TIM_DeInit(TIM3);  //复位时钟tim2
  TIM_TimeBaseStructure.TIM_Period=59;          //计数时间T=[(TIM_Period+1)/Ftosc]*(TIM_Prescaler+1)(Ftosc定时时钟)
	TIM_TimeBaseStructure.TIM_Prescaler=19;       //(Ftosc=48MHz)
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //TIM2时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //计数方式
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);  //初始化	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);            //清除标志
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM1 PWM2 高低电平占空比相反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //PWM输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //PWM的频率 f=Ftosc/(TIM_Period+1)/(TIM_Prescaler+1)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OCInitStructure.TIM_Pulse = 60;   //50%  TIM_Period*50%
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);        //使能中断源
	TIM_Cmd(TIM3,ENABLE);                           //使能TIM2
	LED_PWM_OFF();
}

void PWM_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LD_H_PIN | LD_V_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_2);
}

/*********************************************************************
*功能：定时器初始化
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void TIME_PWM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //定义tim结构体变量
	TIM_OCInitTypeDef  TIM_OCInitStructure;         //PWM结构体
	
	PWM_GPIO_INIT();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); //使能tim2  TIM2被隐藏，规格书中没有，但是能用
	
  TIM_DeInit(TIM1);  //复位时钟tim2
  TIM_TimeBaseStructure.TIM_Period=239;         //计数时间T=[(TIM_Period+1)/Ftosc]*(TIM_Prescaler+1)(Ftosc定时时钟)
	TIM_TimeBaseStructure.TIM_Prescaler=19;       //(Ftosc=48MHz)
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //TIM2时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //计数方式
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);  //初始化	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);            //清除标志
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM1 PWM2 高低电平占空比相反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //PWM输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //PWM的频率 f=Ftosc/(TIM_Period+1)/(TIM_Prescaler+1)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OCInitStructure.TIM_Pulse = 168;   //70%  TIM_Period*70%
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM1,ENABLE);                           //使能TIM2
	TIM_CtrlPWMOutputs(TIM1,DISABLE);                //使能PWM
}

/*********************************************************************
*功能：定时器中断初始化
*参数：无
*返回值： 无
*备注：
*********************************************************************/
void NVIC_TIME_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

