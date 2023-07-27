/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "stm32f4xx_hal.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
/* SPI1 init function */
void MX_SPI1_Init(uint32_t x)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = x;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_Delay(10);
  /* USER CODE BEGIN SPI1_Init 2 */
/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷ */
	
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	  SPI_SendHalfWord(&hspi1,0x80fe);
    HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
	   HAL_Delay( 84 ); 
	 /* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷0£ºADCÅäÖÃºÍ·Å´óÆ÷¿ìËÙ½¨Á¢ */
	 	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	 SPI_SendHalfWord(&hspi1,0x80de);
	 
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷1£º´«¸ÐÆ÷²ÉÑùºÍADC»º³åÆ÷Æ«ÖÃµçÁ÷ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8120);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷2£º´«¸ÐÆ÷²ÉÑùºÍADC»º³åÆ÷Æ«ÖÃµçÁ÷ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8228);

		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
	
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷3£ºÎÂ¶È´«¸ÐÆ÷ºÍ¸¨ÖúÊý×ÖÊä³öÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8301);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
    HAL_Delay( 50 );
	
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷4£ºADCÊä³ö¸ñÊ½ºÍDSPÊ§µ÷Ïû³ýÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8480);

		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
    HAL_Delay( 50 );
		
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷5£º×è¿¹¼ì²â¿ØÖÆ£¨Î´¿ªÆô£©ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8500);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷6£º¼Ä´æÆ÷5µÄDACÉèÖÃ£¨ÖÃÁã£©ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8600);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷7£ºÍ¬ÉÏÖÃÁãÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8700);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷8£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x881e);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷9£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8985);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8985);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷10£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8a2b);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷11£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8b86);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );

	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷12£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8c10);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );	
						
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷13£º·Å´óÆ÷´ø¿íÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8dfc);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷14£º·Å´óÆ÷Æô¶¯ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8eff);
    
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷15£º·Å´óÆ÷Æô¶¯ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x8fff);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷16£º·Å´óÆ÷Æô¶¯ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x90ff);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
	 	/* ³õÊ¼»¯Ð¾Æ¬ÅäÖÃ¼Ä´æÆ÷17£º·Å´óÆ÷Æô¶¯ÉèÖÃ */
	 
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x91ff);
	
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		/* ¼Ä´æÆ÷³õÊ¼»¯ºó100us½øÐÐADCÐ£×¼,Ð£×¼¿ªÊ¼ºóÐèÒª¶îÍâ9ÃüÁîÊ±ÖÓ */
    HAL_Delay( 200 );
		
	  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
	 
	  SPI_SendHalfWord(&hspi1,0x5500);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff0);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		

		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff1);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff2);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff3);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff4);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff5);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff6);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff7);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff8);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
		HAL_Delay( 50 );
		
		
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); HAL_Delay( 5 );
		
		SPI_SendHalfWord(&hspi1,0xfff9);
		
		HAL_Delay( 5 );HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		
    HAL_Delay( 50 );	
}   

uint16_t SPI_SendHalfWord(SPI_HandleTypeDef *hspi1, uint16_t HalfWord)
{
  uint16_t receiveData;
  HAL_SPI_TransmitReceive(hspi1, (uint8_t*)&HalfWord, (uint8_t*)&receiveData, 2, 1000); 
  return receiveData;
}
  /* USER CODE END SPI1_Init 2 */


void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */
	
  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
