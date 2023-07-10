#include "common.h"
#include "stm32f0xx_conf.h"

static INT8U  g_ubMultiple_us = 0;
static INT16U g_uwMultiple_ms = 0;


void Delay(INT16U time)
{
	while(time--);
}


void Delay_Init(INT8U sysclk)
{
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;                   // ѡ��Tickʱ��Դ����ϵͳʱ��
	g_ubMultiple_us = sysclk;
	g_uwMultiple_ms = (INT16U)sysclk * 1000;
}


// ΢�붨ʱ
void Delay_us(INT16U time)
{
	INT32U ulTemp = 0;
	
	SysTick->LOAD = (INT32U)time * g_ubMultiple_us;
	SysTick->VAL = 0;                                             // ��ֵ�����SysTick_CTRL_COUNTFLAG_Msk λ
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;                     // ������ʱ��
	do
	{
		ulTemp = SysTick->CTRL;
	}
	while((ulTemp & SysTick_CTRL_COUNTFLAG_Msk) != SysTick_CTRL_COUNTFLAG_Msk);  // �ȵ��������
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;                                   // ֹͣ��ʱ��
	SysTick->VAL = 0;	
}


// ���붨ʱ
void Delay_ms(INT16U time)
{
	INT32U ulTemp = 0;
	
	SysTick->LOAD = (INT32U)time * g_uwMultiple_ms;
	SysTick->VAL = 0;                                              // ��ֵ�����SysTick_CTRL_COUNTFLAG_Msk λ
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;                      // ������ʱ��
	do
	{
		ulTemp = SysTick->CTRL;
	}
	while((ulTemp & SysTick_CTRL_COUNTFLAG_Msk) != SysTick_CTRL_COUNTFLAG_Msk);  // �ȵ��������
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;                                   // ֹͣ��ʱ��
	SysTick->VAL = 0;
}


