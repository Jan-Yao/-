/* Includes ------------------------------------------------------------------*/
#include "main.h"

int main(void)
{
	Delay_Init(48);
	Init();
	
	while(1)                                                 
	{
		if(Bit_ReceiveCompletion == 1)  //���ڵ���
		{
			DebugCommandProcessing();
			Bit_ReceiveCompletion = 0;
		}
		Sensor_ReadAccel();//����������
		if(System5ms == 1)
		{
			System5ms=0;
			TasksDispatch();
		}
	}
}
