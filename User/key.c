#include "key.h"
#include "pin_mcu.h"
#include "time.h"

uint8_t Keys_Now = 0x00;
uint8_t Keys_Data = 0x00;
uint16_t Key_Delay = 0;

void KeysScan(void)
{
	uint8_t	Key_Temp = 0x00; //��ʱ��λ�洢�����ڴ���ÿ�ν���˳���ʱ��ʵʱ����״̬

	Key_Temp = 0x00;
	Keys_Data = 0x00;

	if(GPIO_ReadInputDataBit(KEY_PULSE_GBIO,KEY_PULSE_PIN) == 0)Key_Temp |= Key_ON;     //8λ���ݿ��Ա�ʾ7�����������λ�������𳤶̰��� 

	if(Key_Temp != 0x00)  //һ
	{
		if(Keys_Now == 0x00)
		{
			Keys_Now = Key_Temp; 
			Key_Delay = 0;
		} 
		else               //��      
		{
			if(Keys_Now == Key_Temp)  //����
			{
				Key_Delay++;
				if(Key_Delay >= 600) //600*5ms = 3s,�������ӳ�
				{
					Keys_Data = Keys_Now | 0x80;  //������ֵ��������ֱ��ִ��
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
	else               //��
	{
		if(Key_Delay > 4 && Key_Delay < 600) //�ɼ�ִ��
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
