#ifndef _USART_H_
#define _USART_H_

#include "stm32f0xx.h"

extern uint8_t RxBuffer[50];
extern uint8_t TxBuffer[50];
extern uint8_t Red_Accel;
extern uint8_t Bit_ReceiveCompletion;
extern uint16_t RxCount;
extern uint16_t NTC_Data;
extern uint16_t TEMP_Data;

void BOARD_Debug_Init(void);
void USARTSendData(uint8_t *pStr);
void DebugCommandProcessing(void);

#endif 
