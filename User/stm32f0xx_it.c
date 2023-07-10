/**
  ******************************************************************************
  * @file    ADC_DMA/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-October-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "key.h"
#include "time.h"
#include "usart.h"
#include "pin_mcu.h"

uint16_t TimeCnt = 0;
uint8_t System5ms = 0;

/** @addtogroup STM32F0308_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup ADC_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0x8.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)  //25us一次中断
{
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)!=RESET)
	{
		TimeCnt++;
		if(TimeCnt >= 200)
		{
			TimeCnt=0;
			System5ms=1;
		}
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	}
}

void USART1_IRQHandler(void)
{
	uint8_t data;

	/* If new data arrived. */
	if (USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART1);
		if(data == '\n')     //结束符
		{
			RxBuffer[RxCount++] = '\n';
			Bit_ReceiveCompletion = 1;
			RxCount = 0;
		}
		else if(Bit_ReceiveCompletion == 0)
		{
			RxBuffer[RxCount++] = data;
		}
	}
}

/**
  * @brief  This function handles External lines 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
//  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
//  { 
//    /* Clear the TAMPER Button EXTI line pending bit */
//    EXTI_ClearITPendingBit(EXTI_Line12);
//  }
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  { 
    /* Clear the TAMPER Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
