 /**
  ******************************************************************************
  * @file    bsp_spi.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./flash/bsp_spi.h"
#include "stdio.h"

 /**
  * @brief  DMA初始化
  */
//#include "./flash/bsp_spi.h"
//#include "stdio.h"
//#include "stm32f4xx_dma.h"

#define	SPI1_DMA


static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/*长度一定要>=数据接收发送大小*/
uint16_t SPI_TX_BUFFER[35] =   {CONVERT0,CONVERT1,CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT32,CONVERT33,CONVERT34};
//uint16_t SPI_TX_BUFFER[1]= {0xfe00};
uint16_t SPI_TX_intan[5] = {0xe800, 0xe900, 0xea00, 0xeb00, 0xec00}; //AScii intan
	
uint16_t SPI_RX_BUFFER[1024];

 /**
  * @brief  SPI初始化
  * @param  无
  * @retval 无
  */
void SPI1_Init(u16 x)
{
	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 使能 SPI 及GPIO 时钟 */
  /*!< SPI_SPI_CS_GPIO, SPI_SPI_MOSI_GPIO, 
       SPI_SPI_MISO_GPIO,SPI_SPI_SCK_GPIO 时钟使能 */
  RCC_AHB1PeriphClockCmd (SPI_SCK_GPIO_CLK | SPI_MISO_GPIO_CLK|SPI_MOSI_GPIO_CLK|CS_GPIO_CLK, ENABLE);

  /*!< SPI_SPI 时钟使能 */
  SPI_CLK_INIT(SPI_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
  //设置引脚复用
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT,SPI_SCK_PINSOURCE,SPI_SCK_AF); 
	GPIO_PinAFConfig(SPI_MISO_GPIO_PORT,SPI_MISO_PINSOURCE,SPI_MISO_AF); 
	GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT,SPI_MOSI_PINSOURCE,SPI_MOSI_AF); 
  
  /*!< 配置 SPI_SPI 引脚: SCK */
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;//PB3
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  
  
  GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_SPI 引脚: MISO */            
  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;    //PB4
  GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_SPI 引脚: MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;      //PB5
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< 配置 SPI_SPI 引脚: CS */                   //PG6
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);

  /* 停止信号 FLASH: CS引脚高电平*/
  SPI_CS_HIGH();

  /* SPI 模式配置 */
  // FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = x;
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI, &SPI_InitStructure);
	
 /* 使能 SPI  */
  
//  SPI1_DMA_Config();
	SPI_Cmd(SPI1, ENABLE);
    SPI_SendHalfWord(0xffff);
}

//void SPI1_DMA_Config(u16 buf)
//{
//  DMA_InitTypeDef DMA_InitStructure;

////	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(DMA2_CLK,ENABLE);
//	
//	/*********************  DMA_Rx == 通道3  数据流2 *********************/
//	DMA_DeInit(DMA2_Stream2);	//DMA2 数据流2   
//	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//通道3 SPI1_RX
//	DMA_InitStructure.DMA_BufferSize=1; //传输数据量的大小
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory; 	//DMA传输方向，spi外设为数据源
//	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_RX_BUFFER;	//数据缓冲区地址
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord ; //内存数据宽度 数据宽度16位
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//传输数据时候内存地址递增
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//内存不循环采集数据
//	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 外设基地址
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //外设数据宽度 数据宽度16位
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//传输地址固定不变  ENABLE时下次发送数据地址递增+1
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//通道优先级                   
//	
//	
//	DMA_Init(DMA2_Stream2,&DMA_InitStructure);		//DMA2  SPI1_RX 
//	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);

//	
////	/*********************  DMA_Tx  *********************/
////	DMA_DeInit(DMA2_Stream3);	//DMA2 数据流3 SPI1_TX 
////	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//通道3 SPI1_TX
////	DMA_InitStructure.DMA_BufferSize=1; //传输数据量的大小
////	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA传输方向，DST 内存作为数据源
////	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_TX_BUFFER;	//数据缓冲区地址
////	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //内存数据宽度 数据宽度16位
////	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//传输数据时候内存地址递增
////	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//内存不循环采集数据
////	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 外设基地址
////	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //外设数据宽度 数据宽度16位
////	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//传输地址固定不变  ENABLE时下次发送数据地址递增+1
////	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//通道优先级
////	
////	DMA_Init(DMA2_Stream3,&DMA_InitStructure);		//DMA2  SPI1_TX 
////	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);//使能DMA2流3的传输完成中断
//////	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;//??1????
//////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//?????3
//////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//????3
//////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ????
//////	NVIC_Init(&NVIC_InitStructure);
////	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);	//使能DMA Tx通道
////	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);	//使能DMA Rx通道

//	/*********************  DMA_Tx  *********************/
//	DMA_DeInit(DMA2_Stream3);	//DMA2 数据流3 SPI1_TX 
//	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//通道3 SPI1_TX
//	DMA_InitStructure.DMA_BufferSize=1; //传输数据量的大小
//	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA传输方向，DST 内存作为数据源
//	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)&buf;	//数据缓冲区地址
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //内存数据宽度 数据宽度16位
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//传输数据时候内存地址递增
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//内存不循环采集数据
//	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 外设基地址
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //外设数据宽度 数据宽度16位
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//传输地址固定不变  ENABLE时下次发送数据地址递增+1
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//通道优先级
////	
//	DMA_Init(DMA2_Stream3,&DMA_InitStructure);		//DMA2  SPI1_TX 
//	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);//使能DMA2流3的传输完成中断
////	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;//??1????
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//?????3
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//????3
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ????
////	NVIC_Init(&NVIC_InitStructure);
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);	//使能DMA Tx通道
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);	//使能DMA Rx通道
//}



//void spi_trans(void)    
//{
//	SPI_Cmd(SPI1, ENABLE);

//	DMA_Cmd(DMA2_Stream2, DISABLE);                                                    
//	DMA_Cmd(DMA2_Stream3, DISABLE); 

////	DMA_SetCurrDataCounter(DMA2_Stream2,NumByte);		//设定通道内存宽度
////	DMA_SetCurrDataCounter(DMA2_Stream3,NumByte);	

////  DMA2_Stream2->M0AR = (uint32_t)rx_buf;                                                     //  ???? ??????????
////	DMA2_Stream3->M0AR = (uint32_t)tx_buf;
//	
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	//等待清除   
//	                                                                                                            
//	DMA_Cmd(DMA2_Stream2, ENABLE);                                                           
//	DMA_Cmd(DMA2_Stream3, ENABLE);                                                        
//	                                                                                       
//	while( DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3) == RESET);   
//	while( DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET); 
//                                                                                                            
//	DMA_Cmd(DMA2_Stream2, DISABLE);                                                         
//	DMA_Cmd(DMA2_Stream3, DISABLE);	    

//    DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);		//清空传输完成flag
//	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
//	
//	
//}

//u16 SPI_DMA_ReadID(void)
//{
//	u16 count = 0;
//	u16 Temp = 0;
//	
//	SPI_CS_LOW();				    
//	SPI_TX_BUFFER[count++]=0xfe00;
//	spi_trans(SPI_TX_BUFFER,SPI_TX_BUFFER,count);
//	SPI_CS_HIGH();
//  Temp = SPI_TX_BUFFER[0];		
//	return Temp;
//	
//}





/*******************************************************************************
* Function Name  : SPI_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/


u16 SPI_SendHalfWord(u16 HalfWord)
{
  
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
   }

  /* Send Half Word through the FLASH_SPI peripheral */
  SPI_I2S_SendData(SPI1, HalfWord);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(3);
   }
  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}




 static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* 等待超时后的处理,输出错误信息 */
  ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
} 
/*********************************************END OF FILE**********************/
