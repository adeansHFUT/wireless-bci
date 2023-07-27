/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.5
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "core_cm4.h"             /* Cortex-M4 processor and core peripherals */
#include "system_stm32f4xx.h"

#ifdef _RTE_
#include "RTE_Components.h"             /* Component selection */
#endif


/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS_RTX
void SVC_Handler(void)
{
}
#endif

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS_RTX
void PendSV_Handler(void)
{
}
#endif

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

//#ifndef RTE_CMSIS_RTOS_RTX
//void SysTick_Handler(void)
//{
//  HAL_IncTick();
//}
//#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
void ConfigurePA0_readin(void)
{
	
	  GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


// TIM3 init

TIM_HandleTypeDef g_timx_handle; 
void gtim_tim3_int_init(void)
{
		ConfigurePA0_readin();
    __HAL_RCC_TIM3_CLK_ENABLE();                             /* ??TIM3?? */
 
    g_timx_handle.Instance = TIM3;                          /* ?????3 */
    g_timx_handle.Init.Prescaler = 8399;                     /* 10khz */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* ?????? */
    g_timx_handle.Init.Period = 1999;                        /* 5hz */
    HAL_TIM_Base_Init(&g_timx_handle);
    
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);      /* ???????,?????1,????3 */
    HAL_NVIC_EnableIRQ(TIM3_IRQn);                 /* ??ITMx?? */
 
    HAL_TIM_Base_Start_IT(&g_timx_handle);         /* ?????x????x???? */

}
uint8_t receive_size;
uint8_t receive_sign;
uint8_t receive_data[10] = {0};
uint8_t link_no = 0;
uint16_t status = 0;
uint8_t maybe_receive = 0;
extern uint8_t first_acquire_circle;

void handle_receive(void)
{
			if(M8266WIFI_SPI_Has_DataReceived())
			{	
				receive_size = M8266WIFI_SPI_RecvData(receive_data, 1, 10,&link_no, &status);
				if(receive_size!=0)
				{
					switch(receive_data[0])
					{
						case 112:
							receive_sign = 1; // send 32 channels;
							first_acquire_circle = 1;
							break;
						case 120:
							receive_sign = 0; // pause
							break;
						case 71:
							receive_sign = 4;
							break;
						case 72:
							receive_sign = 5;
							break;
						case 74:
							receive_sign = 3; // test wifi speed
							break;
						default:
							break;
					}
				}
			}
				
}
void HAL_GPIO_readin_Callback(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
{
    if(HAL_GPIO_ReadPin(GPIO_Port, GPIO_Pin) == GPIO_PIN_SET)  // when IO is high level
    {
			maybe_receive = 1;
		}
}

void TIM3_IRQHandler(void)
{
    /* ???????????HAL??????????,???????????????? */
    if(__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
    {
        HAL_GPIO_readin_Callback(GPIOA, GPIO_PIN_0);
        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);  /* ???????????? */
    }
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
