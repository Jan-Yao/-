#ifndef __LD_H
#define __LD_H		
#include "stm32f0xx.h"

extern uint8_t  Key_ON_F;
extern uint8_t  Key_P_ON;
extern uint8_t  Key_ON_L_F;
extern uint8_t  LD_Limit;
//extern uint8_t  TempDetect;
extern uint16_t System1s;
extern uint16_t Temp_OFF_Cnt;
extern uint16_t LD_OFF_Cnt;

void LD_PWM(void);

#endif
