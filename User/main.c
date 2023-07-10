/* Includes ------------------------------------------------------------------*/
#include "main.h"

int main(void)
{
	Delay_Init(48);
	Init();
	
	while(1)                                                 
	{
		if(Bit_ReceiveCompletion == 1)  //串口调试
		{
			DebugCommandProcessing();
			Bit_ReceiveCompletion = 0;
		}
		Sensor_ReadAccel();//传感器采样
		if(System5ms == 1)
		{
			System5ms=0;
			TasksDispatch();
		}
	}
}
