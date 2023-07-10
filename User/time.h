#ifndef __time_H
#define __time_H		
#include "stm32f0xx.h"

void TIME_Configuration(void);
void NVIC_TIME_Configuration(void);
void PWM_GPIO_INIT(void);
void TIME_PWM_Configuration(void);
void LED_PWM_OFF(void);
void LED_H_PWM_ON(void);
void LED_V_PWM_ON(void);
void LED_HV_PWM_ON(void);
void SET_TIM3_PWM_NUM(uint16_t Compare1);
uint16_t RED_TIM3_PWM_NUM(void);
									
#endif
