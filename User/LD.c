#include "LD.h"
#include "pin_mcu.h"

uint8_t  ADC_OFF = 0;
uint16_t LD_LOCK_NUM = 0;
uint16_t LD_Limit_NUM = 0;
uint16_t LD_LOCK_NUM_LP = 0;
uint16_t LD_UNLOCK_NUM_LP = 0;

void LD_PWM(void)
{
//	if(TempDetect == 2)
//	{
//		if(((Temp_OFF_Cnt > 0) && (Temp_OFF_Cnt <= 25)) || ((Temp_OFF_Cnt > 50) && (Temp_OFF_Cnt <= 75))
//		|| ((Temp_OFF_Cnt > 100) && (Temp_OFF_Cnt <= 125)) || ((Temp_OFF_Cnt > 150) && (Temp_OFF_Cnt <= 175))
//		|| ((Temp_OFF_Cnt > 200) && (Temp_OFF_Cnt <= 225)) || ((Temp_OFF_Cnt > 250) && (Temp_OFF_Cnt <= 275))
//		|| ((Temp_OFF_Cnt > 300) && (Temp_OFF_Cnt <= 325)) || ((Temp_OFF_Cnt > 350) && (Temp_OFF_Cnt <= 375))
//		|| ((Temp_OFF_Cnt > 400) && (Temp_OFF_Cnt <= 425)) || ((Temp_OFF_Cnt > 450) && (Temp_OFF_Cnt <= 475))
//		|| ((Temp_OFF_Cnt > 500) && (Temp_OFF_Cnt <= 525)) || ((Temp_OFF_Cnt > 550) && (Temp_OFF_Cnt <= 575)))
//		{
//			ADC_OFF = 1;
//			TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
//			TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
//		}
//		else
//		{
//			ADC_OFF = 0;
//			if(Key_ON_F == 1)   //H亮
//			{
//				if(TIM1->CCR1 != 120)   //30%PWM 
//				{
//					TIM1->CCER = 0X0004;
//				}
//				else                      
//				{                                                   //50%PWM
//					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
//					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
//				}
//			}		
//			else if(Key_ON_F == 2) //V亮
//			{
//				if(TIM1->CCR1 != 120)
//				{
//					TIM1->CCER = 0X0009;
//				}
//				else
//				{
//					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
//					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
//				}
//			}	
//			else if(Key_ON_F == 0)  //HV同时亮
//			{
//				if(TIM1->CCR1 != 120)
//				{
//					TIM1->CCER = 0X000D;
//				}
//				else
//				{
//					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
//					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
//				}
//			}
//		}
//	}
	if(LD_OFF_Cnt != 0)
	{
		if(((LD_OFF_Cnt > 0) && (LD_OFF_Cnt <= 100)) 
		|| ((LD_OFF_Cnt > 200) && (LD_OFF_Cnt <= 300))
		|| ((LD_OFF_Cnt > 400) && (LD_OFF_Cnt <= 500)))
		{
			ADC_OFF = 1;
			TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
			TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
		}
		else
		{
			ADC_OFF = 0;
			if(Key_ON_F == 1)   //H亮
			{
				if(TIM1->CCR1 != 120)   
				{
					TIM1->CCER = 0X0004;
				}
				else                      
				{                                                   //50%PWM
					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
				}
			}		
			else if(Key_ON_F == 2) //V亮
			{
				if(TIM1->CCR1 != 120)
				{
					TIM1->CCER = 0X0009;
				}
				else
				{
					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
				}
			}	
			else if(Key_ON_F == 0)  //HV同时亮
			{
				if(TIM1->CCR1 != 120)
				{
					TIM1->CCER = 0X000D;
				}
				else
				{
					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
				}
			}
		}
	}
	else
	{
		if(((LD_Limit == 1) && (Key_P_ON == 1)) && (GPIO_ReadInputDataBit(LOCK_GBIO,LOCK_PIN) == 1))
		{
			LD_LOCK_NUM = 0;
			LD_Limit_NUM++;
			if(LD_Limit_NUM > 195) //1s
			{
				LD_Limit_NUM = 0;
			}
		}
		else if(Key_ON_L_F == 5)  //低压报警
		{
			if((GPIO_ReadInputDataBit(LOCK_GBIO,LOCK_PIN) == 0))
			{
				LD_UNLOCK_NUM_LP = 0;
				LD_LOCK_NUM_LP++;
				if(LD_LOCK_NUM_LP > 1795)
				{
					LD_LOCK_NUM_LP = 0;
				}
			}
			else
			{
				LD_LOCK_NUM_LP = 0;
				LD_UNLOCK_NUM_LP++;
				if(LD_UNLOCK_NUM_LP > 995)
				{
					LD_UNLOCK_NUM_LP = 0;
				}
			}
		}
		else if((GPIO_ReadInputDataBit(LOCK_GBIO,LOCK_PIN) == 0) && (Key_P_ON == 1) && ((LD_Limit == 0)))  //不锁紧 开机 不限位
		{
			System1s = 0;
			LD_LOCK_NUM++;
			if(LD_LOCK_NUM > 1700) //8.5s
			{
				LD_LOCK_NUM = 0;
			}
		}
		else
		{
			LD_LOCK_NUM_LP = 0;
			LD_UNLOCK_NUM_LP = 0;
			LD_Limit_NUM = 0;
			LD_LOCK_NUM = 0;
		}
		
		if(LD_Limit == 1)   //感应
		{
			if(((LD_Limit_NUM > 0) && (LD_Limit_NUM <= 35)) 
			|| ((LD_Limit_NUM > 65) && (LD_Limit_NUM <= 100))
			|| ((LD_Limit_NUM > 130) && (LD_Limit_NUM <= 165))) 
			{
				ADC_OFF = 1;
				TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
				TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
			}
			else
			{
				ADC_OFF = 0;
				if(Key_ON_F == 1)   //H亮
				{
					if(TIM1->CCR1 != 120)   //30%PWM 
					{
						TIM1->CCER = 0X0004;
					}
					else                      
					{                                                   //50%PWM
						TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
						TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
					}
				}		
				else if(Key_ON_F == 2) //V亮
				{
					if(TIM1->CCR1 != 120)
					{
						TIM1->CCER = 0X0009;
					}
					else
					{
						TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
						TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
					}
				}	
				else if(Key_ON_F == 0)  //HV同时亮
				{
					if(TIM1->CCR1 != 120)
					{
						TIM1->CCER = 0X000D;
					}
					else
					{
						TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
						TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
					}
				}
			}
		}
		else
		{
			if(Key_ON_L_F == 5)  //低电压报警 8S常亮 1s内开关循环3次
			{
				if((LD_LOCK_NUM_LP != 0) && (LD_UNLOCK_NUM_LP == 0))
				{
					if(((LD_LOCK_NUM_LP > 1600) && (LD_LOCK_NUM_LP <= 1635)) 
					|| ((LD_LOCK_NUM_LP > 1665) && (LD_LOCK_NUM_LP <= 1700))
					|| ((LD_LOCK_NUM_LP > 1730) && (LD_LOCK_NUM_LP <= 1765)))  
					{
						ADC_OFF = 1;
						TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
						TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
					}
					else
					{
						ADC_OFF = 0;
						if(Key_ON_F == 1)   //H亮
						{
							if(TIM1->CCR1 != 120)   
							{
								TIM1->CCER = 0X0004;
							}
							else                      
							{                                                   //50%PWM
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
							}
						}		
						else if(Key_ON_F == 2) //V亮
						{
							if(TIM1->CCR1 != 120)
							{
								TIM1->CCER = 0X0009;
							}
							else
							{
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
							}
						}	
						else if(Key_ON_F == 0)  //HV同时亮
						{
							if(TIM1->CCR1 != 120)
							{
								TIM1->CCER = 0X000D;
							}
							else
							{
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
							}
						}
					}
				}
				else if((LD_UNLOCK_NUM_LP != 0) && ((LD_LOCK_NUM_LP == 0)))
				{
					if(((LD_UNLOCK_NUM_LP > 800) && (LD_UNLOCK_NUM_LP <= 835)) 
					|| ((LD_UNLOCK_NUM_LP > 865) && (LD_UNLOCK_NUM_LP <= 900))
					|| ((LD_UNLOCK_NUM_LP > 930) && (LD_UNLOCK_NUM_LP <= 965)))  
					{
						ADC_OFF = 1;
						TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
						TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
					}
					else
					{
						ADC_OFF = 0;
						if(Key_ON_F == 1)   //H亮
						{
							if(TIM1->CCR1 != 120)   
							{
								TIM1->CCER = 0X0004;
							}
							else                      
							{                                                   //50%PWM
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
							}
						}		
						else if(Key_ON_F == 2) //V亮
						{
							if(TIM1->CCR1 != 120)
							{
								TIM1->CCER = 0X0009;
							}
							else
							{
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
							}
						}	
						else if(Key_ON_F == 0)  //HV同时亮
						{
							if(TIM1->CCR1 != 120)
							{
								TIM1->CCER = 0X000D;
							}
							else
							{
								TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
								TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
							}
						}
					}
				}
			}
			else   //LCOK 闪烁 8S常亮 8-
			{
				if((LD_LOCK_NUM > 1600) && (LD_LOCK_NUM <= 1700))  //8s~8.5s关激光
				{
					ADC_OFF = 1;
					TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
					TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
				}
				else
				{
					ADC_OFF = 0;
					if(Key_ON_F == 1)   //H亮
					{
						if(TIM1->CCR1 != 120)   
						{
							TIM1->CCER = 0X0004;
						}
						else                      
						{                                                   //50%PWM
							TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);   //CH1     LD_V 
							TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);  //CH1_N 
						}
					}		
					else if(Key_ON_F == 2) //V亮
					{
						if(TIM1->CCR1 != 120)
						{
							TIM1->CCER = 0X0009;
						}
						else
						{
							TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
							TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
						}
					}	
					else if(Key_ON_F == 0)  //HV同时亮
					{
						if(TIM1->CCR1 != 120)
						{
							TIM1->CCER = 0X000D;
						}
						else
						{
							TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
							TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);								
						}
					}
				}
			}
		}
	}
}
