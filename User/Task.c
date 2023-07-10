#include "Task.h"
#include "GPIO.h"
#include "Flash.h"
#include "Time.h"
#include "ADC.h"
#include "Charging.h"
#include "pin_mcu.h"
#include "key.h"
#include "LD.h"

uint8_t LD_Mode = 0;
uint8_t LD_PULSE = 0;
uint16_t OffBvcCnt = 0;
uint8_t ChargeSleepCnt = 0;
uint8_t Key_P_ON = 0;
uint8_t Key_ON_F = 0;
uint8_t Key_ON_L_F = 0;
uint8_t LED_PWM_NUM = 0;
uint8_t LED_PWM_MODE = 0;
uint8_t System40ms = 0;
uint8_t Switch_ON = 0;
uint8_t Vol_Delay_ON = 0;
uint16_t LD_OFF_Cnt = 0;
uint16_t Sleep_ON_Cnt = 0;
uint16_t Sleep_OFF_Cnt = 0;

static void (*TasksArray[SCH_MAX_TASKS])(void) ;

/**
  * @brief  ����ָʾLED �����ƹ���
  * @param  None
  * @retval None
  */
void LED_Model_Breathe(void)
{
	if((RED_TIM3_PWM_NUM() != 60) && ((Key_ON_L_F == 1) || (Key_ON_L_F == 2)))  //L_MODE�����µķ�LOWPģʽ
	{
		System40ms++;
		if(System40ms >= 8)
		{
			System40ms=0;
			LED_PWM_NUM = RED_TIM3_PWM_NUM();
			if(LED_PWM_NUM == 48)
			{
				LED_PWM_MODE = 1;  //��ģʽ
			}
			else if(LED_PWM_NUM == 12)
			{
				LED_PWM_MODE = 2;  //��ģʽ
			}
			if((LED_PWM_NUM > 12) && (LED_PWM_MODE == 1))
			{
				LED_PWM_NUM--;
			}
			else if((LED_PWM_NUM < 48) && (LED_PWM_MODE == 2))
			{
				LED_PWM_NUM++;
			}
			SET_TIM3_PWM_NUM(LED_PWM_NUM);
		}
	}
	if((RED_TIM3_PWM_NUM() != 60) && (Key_ON_L_F == 5))
	{
		if(ADC_OFF == 1)
		{
			LED_PWM_OFF();
		}
		else
		{
			if(Key_ON_F == 1)
			{
				LED_H_PWM_ON();
			}
			else if(Key_ON_F == 2)
			{
				LED_V_PWM_ON();
			}
			else if(Key_ON_F == 0)
			{
				LED_HV_PWM_ON(); 
			}
		}
	}
}

/**
  * @brief  ���ػ�״̬
  * @param  None
  * @retval None
  */
void Switch_Status(void)
{
	if(Key_P_ON == 0)  //������
	{
		if(GPIO_ReadInputDataBit(KEY_P_GBIO,KEY_P_PIN) == 1)
		{
			Switch_ON = 1;
			if(Vol_Delay_ON == 1)
			{
				if(BatData > Bat3v20)
				{
					LD_OFF_Cnt = 0;
					Key_P_ON = 1;
					Vol_Delay_ON = 0;
					TIM_CtrlPWMOutputs(TIM1,ENABLE); //LD
					LED_HV_PWM_ON();                 //LED_MODE      ����
				}
			}
			else
			{
				if(BatData > Bat2v70)
				{
					LD_OFF_Cnt = 0;
					Key_P_ON = 1;
					TIM_CtrlPWMOutputs(TIM1,ENABLE); //LD
					LED_HV_PWM_ON();                 //LED_MODE      ����
				}
			}
		}
		else
		{
			if(GPIO_ReadInputDataBit(KEY_P_GBIO,KEY_P_PIN) == 0)
			{
				LD_OFF_Cnt = 0;
				Switch_ON = 0;
			}
		}
	}				
	else
	{
		if((GPIO_ReadInputDataBit(KEY_P_GBIO,KEY_P_PIN) == 0) || (BatData <= Bat2v70))
		{
			if(BatData <= Bat2v70) //�ŵ��LD �ص�ʱΪ�˷�ֹ�ظ����أ���Ҫ�ָ���3.35V
			{
				Vol_Delay_ON = 1;
			}
			else
			{
				if((Vol_Delay_ON == 1) && (LD_OFF_Cnt >= 600)) 
				{
					Vol_Delay_ON = 0;
				}
			}
			if(Vol_Delay_ON == 1)
			{
				LD_OFF_Cnt++;
				if(LD_OFF_Cnt > 600)
				{
					LD_OFF_Cnt = 600;
					Key_P_ON = 0;
			
					TIM_CtrlPWMOutputs(TIM1,DISABLE);
					Key_ON_F = 0;   //LD_MODE     ��0
					TIM_OC1NPolarityConfig(TIM1,TIM_OCNPolarity_High); //ռ�ձȻָ�
					TIM_SetCompare1(TIM1,168);
					Key_ON_L_F = 0; //LD_LOW_MODE ��0
					SET_TIM3_PWM_NUM(60);
					LED_PWM_MODE = 0; //�����Ʋ�����0
					System40ms = 0;   //��ʱ������0
					LED_PWM_OFF();
				}
			}
			else
			{
				Key_P_ON = 0;
			
				TIM_CtrlPWMOutputs(TIM1,DISABLE);
				Key_ON_F = 0;   //LD_MODE     ��0
				TIM_OC1NPolarityConfig(TIM1,TIM_OCNPolarity_High); //ռ�ձȻָ�
				TIM_SetCompare1(TIM1,168);
				Key_ON_L_F = 0; //LD_LOW_MODE ��0
				SET_TIM3_PWM_NUM(60);
				LED_PWM_MODE = 0; //�����Ʋ�����0
				System40ms = 0;   //��ʱ������0
				LED_PWM_OFF();
			}
		}
		else 
		{
			if(BatData <= Bat3v00) 
			{
				Key_ON_L_F = 5;
				TIM_SetCompare1(TIM1,168);
				SET_TIM3_PWM_NUM(30);  //50%����
			}
		}
	}
}

/**
  * @brief  ��ѹ����
  * @param  None
  * @retval None
  */
void Bat_ADC(void)
{
	BVCON;
	if(ADC_OFF == 0)
	{
		BatVfilter();	
	}
}

/**
  * @brief  ����״̬
  * @param  None
  * @retval None
  */
void Key_Status(void)
{
	KeysScan();
	switch(Keys_Data)   //��������
	{
		case Key_ON:
			if(Key_P_ON == 1)
			{
				if(Key_ON_F == 0)
				{
					Key_ON_F = 1;
					LED_H_PWM_ON();
				}
				else if(Key_ON_F == 1)
				{
					Key_ON_F = 2;
					LED_V_PWM_ON();
				}
				else if(Key_ON_F == 2)
				{ 
					Key_ON_F = 0;
					LED_HV_PWM_ON(); 
				}
			}
			break;
		case Key_ON_L:
			if(Key_P_ON == 1)
			{
				if(Key_ON_L_F == 0)
				{
					TIM_OC1NPolarityConfig(TIM1,TIM_OCNPolarity_Low);
					TIM_SetCompare1(TIM1,120);
					SET_TIM3_PWM_NUM(48);
					Key_ON_L_F = 1;
				}
				else if(Key_ON_L_F == 2)
				{
					TIM_OC1NPolarityConfig(TIM1,TIM_OCNPolarity_High);
					TIM_SetCompare1(TIM1,168);
					SET_TIM3_PWM_NUM(60);
					LED_PWM_MODE = 0; //�����Ʋ�����0
					System40ms = 0;   //��ʱ������0
					Key_ON_L_F = 3;
				}
			}
			break;
	}
}

/**
  * @brief  ϵͳStopģʽ
  * @param  None
  * @retval None
  */
void Model_Stop(void)
{
//	if((FusbTestOver==1)&&(FadapterNo==1))  //˯��(stopģʽ)
//	{
		if(Key_P_ON == 0)
		{
			if(Switch_ON == 1)
			{
				Sleep_OFF_Cnt=0;
				Sleep_ON_Cnt++;
				if(Sleep_ON_Cnt > 1000)  //5s
				{
					Sleep_ON_Cnt=0;
					Sleep_Mode();
					
					SystemInit();
					Init();
					EXTI_DeInit();
				}
			}
			else if(Switch_ON == 0)
			{
				Sleep_ON_Cnt=0;
				Sleep_OFF_Cnt++;
				if(Sleep_OFF_Cnt > 1000)  //5s
				{
					Sleep_OFF_Cnt=0;
					Sleep_Mode();
					
					SystemInit();
					Init();
					EXTI_DeInit();
				}
			}
		}
		else
		{
			Sleep_ON_Cnt=0;
			Sleep_OFF_Cnt=0;
		}
//	}
}

/**
  * @brief  ����ָ�봴��
  * @param  None
  * @retval None
  */
void SetTimerTask(const uint32_t index,void (* pFunction)())
{
	// ��ʱ�������Ч
  if (index <= SCH_MAX_TASKS)
  {
	  // ��ӵ���ʱ���¼��б��ָ��λ��
	  TasksArray[index] = pFunction;
  }
}

/**
  * @brief  ��������
  * @param  None
  * @retval None
  */
void Tasks_Load(void)
{
	SetTimerTask(Task_LED_Model_Breathe,LED_Model_Breathe);
	SetTimerTask(Task_Switch_Status,Switch_Status);
	SetTimerTask(Task_Bat_ADC,Bat_ADC);
	SetTimerTask(Task_Key_Status,Key_Status);
	SetTimerTask(Task_Limit,Limit);
	SetTimerTask(Task_LD_PWM,LD_PWM);
	SetTimerTask(Task_DisplayProcess,DisplayProcess);
	SetTimerTask(Task_Model_Stop,Model_Stop);
}

/**
  * @brief  �������
  * @param  None
  * @retval None
  */
void Tasks_Empty(void)
{
	uint32_t i;
	for (i = 0; i <= SCH_MAX_TASKS; i++)
	{
		TasksArray[i] = 0;  // ���
	}
}

/**
  * @brief  ����ִ��
  * @param  None
  * @retval None
  */
void TasksDispatch(void)
{
	uint32_t i;
	for (i = 0; i <= SCH_MAX_TASKS; i++)
	{
		(*TasksArray[i])();  // ִ�и�����
	}
}

/**
  * @brief  �����ʼ��
  * @param  None
  * @retval None
  */
void Init(void)
{
	GPIO_Configuration();
	BOARD_Debug_Init();
	Pflash_Init();
	Accelerometer_init();
	TIME_Configuration();
	NVIC_TIME_Configuration();
//	I2cInit();
	ADC_Configuration();
	TIME_PWM_Configuration();
	Tasks_Load();
}
