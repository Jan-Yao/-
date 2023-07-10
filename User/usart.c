#include "usart.h"
#include "pin_mcu.h"
#include "ADC.h"
#include "Flash.h"
#include "Sensor_I2C.h"
#include <stdio.h>

uint8_t RxBuffer[50];
uint8_t TxBuffer[50];
uint8_t Red_Accel = 0;
uint16_t RxCount = 0;
uint8_t Bit_ReceiveCompletion = 0;

/**
  * @brief	Initialize debug console.	
  * @param	None
  * @retva	None
  */

void USART_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
	
	/*USART TX RX */
	GPIO_uInitStructure.GPIO_Pin = D_TX_PIN | D_RX_PIN;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP; 
	//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
}

void BOARD_Debug_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_GPIO_INIT();
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
}

void USARTSendData(uint8_t *pStr)
{
	while(*pStr)
	{
		while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));
		USART_SendData(USART1,*pStr);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)); 
		pStr++;
	}
}

/**
  * @brief	调试命令解析
  * @param	None
  * @retva	None
  */
void DebugCommandProcessing(void)
{
  uint8_t cmd;
	
  cmd = RxBuffer[0];
  
  switch(cmd)
  {
		case 'R':      //读取加速度值
			if(Red_Accel == 0)
			{
				Red_Accel = 1;
			}
			else
			{
				sprintf((char *)TxBuffer,"AngelZero_X = %d\r\n",SysPar.SysStruct.AngelZero_X);
				USARTSendData(TxBuffer);
				sprintf((char *)TxBuffer,"AngelZero_Y = %d\r\n",SysPar.SysStruct.AngelZero_Y);
				USARTSendData(TxBuffer);
				sprintf((char *)TxBuffer,"AngelZero_Z = %d\r\n",SysPar.SysStruct.AngelZero_Z);
				USARTSendData(TxBuffer);
				Red_Accel = 0;
			}
      break;
    case 'V':      //标定电压
			SysPar.SysStruct.Bat_V_Error = (BatData - 450);
		  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
      SaveInformation();
		  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
			sprintf((char *)TxBuffer,"BatData = %d\r\n",BatData);
		  USARTSendData(TxBuffer);
		  sprintf((char *)TxBuffer,"Error = %d\r\n",SysPar.SysStruct.Bat_V_Error);
		  USARTSendData(TxBuffer);
      break;
    case 'S':      //零位标定   
			SetAngelZero();
		  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
      SaveInformation();
		  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		  sprintf((char *)TxBuffer,"OK");
      USARTSendData(TxBuffer);
      break;
		case 'T':      //温度标定 
		  sprintf((char *)TxBuffer,"NTC = %d\r\n",NTC_Data);
		  USARTSendData(TxBuffer);
		  sprintf((char *)TxBuffer,"Temp = %d\r\n",TEMP_Data);
		  USARTSendData(TxBuffer);
      break;
    case 'F':      //读取Flash数据   
      break;
  }
}
