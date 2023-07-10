#ifndef __Task_H_
#define __Task_H_  

#include "usart.h"
#include "Sensor_I2C.h"

#define SCH_MAX_TASKS 7 //最大支持定时时间数

//任务枚举
enum
{
	Task_LED_Model_Breathe = 0,		//功能指示LED 呼吸灯功能
	Task_Switch_Status,           //开关机状态
	Task_Bat_ADC,                 //电压采样
	Task_Key_Status,              //按键状态
	Task_Limit,
	Task_LD_PWM,
	Task_DisplayProcess,
	Task_Model_Stop,
};

void Tasks_Load(void);
void TasksDispatch(void);
void Init(void);
	
#endif
