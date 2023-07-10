#include "key.h"
#include "pin_mcu.h"
#include "time.h"

uint8_t Keys_Now = 0x00;
uint8_t Keys_Data = 0x00;
uint16_t Key_Delay = 0;

void KeysScan(void)
{
	uint8_t	Key_Temp = 0x00; //临时键位存储，用于储存每次进入此程序时的实时按键状态

	Key_Temp = 0x00;
	Keys_Data = 0x00;

	if(GPIO_ReadInputDataBit(KEY_PULSE_GBIO,KEY_PULSE_PIN) == 0)Key_Temp |= Key_ON;     //8位数据可以表示7个按键，最高位用来区别长短按键 

	if(Key_Temp != 0x00)  //一
	{
		if(Keys_Now == 0x00)
		{
			Keys_Now = Key_Temp; 
			Key_Delay = 0;
		} 
		else               //二      
		{
			if(Keys_Now == Key_Temp)  //消抖
			{
				Key_Delay++;
				if(Key_Delay >= 600) //600*5ms = 3s,长按键延迟
				{
					Keys_Data = Keys_Now | 0x80;  //长按赋值，跳出后直接执行
					Key_Delay = 600;
				}
			}
			else
			{
				Keys_Now = Key_Temp;
				Key_Delay = 0;
			}
		}
	} 
	else               //三
	{
		if(Key_Delay > 4 && Key_Delay < 600) //松键执行
		{
			Keys_Data = Keys_Now;
		}
		else if(Key_Delay == 600)
		{
			if(Key_ON_L_F == 1)
			{
				Key_ON_L_F = 2;
			}
			else if(Key_ON_L_F == 3)
			{
				Key_ON_L_F = 0;
			}
		}
		Keys_Now = 0;
		Key_Delay = 0;
	}
}
