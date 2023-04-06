#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stm32f4xx.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define CONVERT0                0x0000 //ADC CONVERT COMMAND
#define CONVERT1                0x0100
#define CONVERT2                0x0200
#define CONVERT3                0x0300
#define CONVERT4                0x0400
#define CONVERT5                0x0500
#define CONVERT6                0x0600
#define CONVERT7                0x0700
#define CONVERT8                0x0800
#define CONVERT9                0x0900
#define CONVERT10               0x0a00
#define CONVERT11               0x0b00
#define CONVERT12               0x0c00
#define CONVERT13               0x0d00
#define CONVERT14               0x0e00
#define CONVERT15               0x0f00
#define CONVERT16               0x1000
#define CONVERT17               0x1100
#define CONVERT18               0x1200
#define CONVERT19               0x1300
#define CONVERT20               0x1400
#define CONVERT21               0x1500
#define CONVERT22               0x1600
#define CONVERT23               0x1700
#define CONVERT24               0x1800
#define CONVERT25               0x1900
#define CONVERT26               0x1a00
#define CONVERT27               0x1b00
#define CONVERT28               0x1c00
#define CONVERT29               0x1d00
#define CONVERT30               0x1e00
#define CONVERT31               0x1f00


#define CONVERT32               0x2000 //auxiliary command
#define CONVERT33               0x2100
#define CONVERT34               0x2200


#define CS_PIN                        GPIO_Pin_4               
#define CS_GPIO_PORT                  GPIOA                     
#define CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA

#define SPI_CS_LOW()      {CS_GPIO_PORT->BSRRH=CS_PIN;}
#define SPI_CS_HIGH()     {CS_GPIO_PORT->BSRRL=CS_PIN;}
 	    													  
void SPI1_Init(u16 x);			 //初始化SPI1口
u16 SPI_SendHalfWord(u16 HalfWord);
		 
#endif

