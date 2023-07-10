#ifndef __key_H
#define __key_H		
#include "stm32f0xx.h"

#define Key_ON        0x01
#define Key_ON_L      0x81

extern uint8_t Keys_Data;
extern uint8_t LD_Mode;
extern uint8_t Key_ON_L_F;

void KeysScan(void);
	
#endif
