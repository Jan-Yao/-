#ifndef __ADC_H
#define __ADC_H		
#include "stm32f0xx.h"

extern uint16_t  BatVCount;
extern uint16_t  BatSum;
extern uint16_t  BatData;

void ADC_Configuration(void);
uint16_t Get_Adc(unsigned char ch);
void BatVfilter(void);

#endif
