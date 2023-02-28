#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h"
#include <stdio.h>

#define SPI1_DR_Addr (uint32_t)0x4001300C 
/*长度一定要>=数据接收发送大小*/
#define DMA_CLK      RCC_AHB1Periph_DMA2
#define a 		    	(uint8_t)35

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
#define intan_ID		            0xff00 

#define CALIBRATE		            0x5500  //CALIBRATE ADC

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

/*命令定义-结尾*******************************/
/*DMA接口定义-开头****************************/

#define DMA2_CLK                      RCC_AHB1Periph_DMA2

/*SPI接口定义-开头****************************/
#define SPI                           SPI1
#define SPI_CLK                       RCC_APB2Periph_SPI1
#define SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define SPI_SCK_PIN                   GPIO_Pin_3                  
#define SPI_SCK_GPIO_PORT             GPIOB                       
#define SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPI_SCK_PINSOURCE             GPIO_PinSource3
#define SPI_SCK_AF                    GPIO_AF_SPI1

#define SPI_MISO_PIN                  GPIO_Pin_4                
#define SPI_MISO_GPIO_PORT            GPIOB                   
#define SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI_MISO_PINSOURCE            GPIO_PinSource4
#define SPI_MISO_AF                   GPIO_AF_SPI1

#define SPI_MOSI_PIN                  GPIO_Pin_5                
#define SPI_MOSI_GPIO_PORT            GPIOB                     
#define SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define SPI_MOSI_AF                   GPIO_AF_SPI1

#define CS_PIN                        GPIO_Pin_6               
#define CS_GPIO_PORT                  GPIOG                     
#define CS_GPIO_CLK                   RCC_AHB1Periph_GPIOG

#define SPI_CS_LOW()      {CS_GPIO_PORT->BSRRH=CS_PIN;}
#define SPI_CS_HIGH()     {CS_GPIO_PORT->BSRRL=CS_PIN;}
/*SPI接口定义-结尾****************************/

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*信息输出*/
#define DEBUG_ON         1
#define ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)



//void SPI1_Init(void);
void SPI1_Init(u16 x);
//void SPI1_DMA_Config(void);
void SPI1_DMA_Config(u16 buf);
void spi_trans(void) ;
u16 SPI_DMA_ReadID(void);
void DMA2_Stream3_IRQHandler(void);
void SPI_DMA_WRITE_READ_BUF(void);
void DMA_Write_buf(uint32_t SizeLen);	
u16 SPI_SendHalfWord(u16 HalfWord);
void SPI_WriteEnable(void);
void SPI_WaitForWriteEnd(void);


#endif /* __SPI_H */

