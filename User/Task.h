#ifndef __Task_H_
#define __Task_H_  

#include "usart.h"
#include "Sensor_I2C.h"

#define SCH_MAX_TASKS 7 //���֧�ֶ�ʱʱ����

//����ö��
enum
{
	Task_LED_Model_Breathe = 0,		//����ָʾLED �����ƹ���
	Task_Switch_Status,           //���ػ�״̬
	Task_Bat_ADC,                 //��ѹ����
	Task_Key_Status,              //����״̬
	Task_Limit,
	Task_LD_PWM,
	Task_DisplayProcess,
	Task_Model_Stop,
};

void Tasks_Load(void);
void TasksDispatch(void);
void Init(void);
	
#endif
