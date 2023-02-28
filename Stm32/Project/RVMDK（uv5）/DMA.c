#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "./flash/bsp_spi.h"

#define Rx_BFSize  (u16)4096
#define Tx_BFSize  (u16)4096
/*长度一定要>=数据接收发送大小*/
uint16_t SPI_RX_BUFFER[Rx_BFSize];	
uint16_t SPI_TX_BUFFER[Tx_BFSize];


void SPI1_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(DMA2_CLK,ENABLE);
	
	/*********************  DMA_Rx == 通道3  数据流2 *********************/
	DMA_DeInit(DMA2_Stream2);	//DMA2 数据流2   
	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//通道3 SPI1_RX
	DMA_InitStructure.DMA_BufferSize=16; //传输数据量的大小
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory; 	//DMA传输方向，spi外设为数据源
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_RX_BUFFER;	//数据缓冲区地址
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord ; //内存数据宽度 数据宽度16位
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//传输数据时候内存地址递增
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//内存不循环采集数据
	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 外设基地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //外设数据宽度 数据宽度16位
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//传输地址固定不变  ENABLE时下次发送数据地址递增+1
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//通道优先级                   
	
	DMA_Init(DMA2_Stream2,&DMA_InitStructure);		//DMA2  SPI1_RX 
	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC | DMA_IT_TE, ENABLE);
  // 中断初始化 
  /* 配置 DMA接收为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;  	
  /* 抢断优先级 */  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		
  /* 响应优先级 */  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				
  /* 使能外部中断通道 */  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 配置NVIC */	
  NVIC_Init(&NVIC_InitStructure);

	
	
	/*********************  DMA_Tx  *********************/
	DMA_DeInit(DMA2_Stream3);	//DMA2 数据流3 SPI1_TX 
	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//通道3 SPI1_TX
	DMA_InitStructure.DMA_BufferSize=16; //传输数据量的大小
	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA传输方向，DST 内存作为数据源
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_TX_BUFFER;	//数据缓冲区地址
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //内存数据宽度 数据宽度16位
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//传输数据时候内存地址递增
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//内存不循环采集数据
	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 外设基地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //外设数据宽度 数据宽度16位
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//传输地址固定不变  ENABLE时下次发送数据地址递增+1
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//通道优先级
	
	DMA_Init(DMA2_Stream3,&DMA_InitStructure);		//DMA2  SPI1_TX 
	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC | DMA_IT_TE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;//??1????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//?????3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ????
	NVIC_Init(&NVIC_InitStructure);
  
}

void DMA2_Stream3_IRQHandler(void)
{
    // DMA 发送完成
    if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))	
    {
        // 清除DMA发送完成标志
        DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);	
        // 片选拉高，数据发送完毕	
        SPI_CS_HIGH();	
    }
}

void DMA2_Stream2_IRQHandler(void)
{				
    // DMA接收完成
    if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))	
    {		
        // 数据接收完成 拉高片选
        SPI_CS_HIGH();	
        // 清除DMA接收完成标志位		
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);	
    }
}


void spi_trans(u16 *rx_buf,u16 *tx_buf,u16 NumByte)    
{
  SPI_Cmd(SPI1, ENABLE);
	DMA_Cmd(DMA2_Stream2, DISABLE);                                                    
	DMA_Cmd(DMA2_Stream3, DISABLE); 

	DMA_SetCurrDataCounter(DMA2_Stream2,NumByte);		//设定通道内存宽度
	DMA_SetCurrDataCounter(DMA2_Stream3,NumByte);	

  DMA2_Stream2->M0AR = (uint32_t)rx_buf;                                                     //  ???? ??????????
	DMA2_Stream3->M0AR = (uint32_t)tx_buf;

  SPI1->DR;	
                                                                                                                                                                                                                                                           
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	//等待清除   
	                                                                                                            
	DMA_Cmd(DMA2_Stream2, ENABLE);                                                           
	DMA_Cmd(DMA2_Stream3, ENABLE);                                                        
	                                                                                       
	while( DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3) == RESET);   
	while( DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET);    
	                                                                                                            
	DMA_Cmd(DMA2_Stream2, DISABLE);                                                         
	DMA_Cmd(DMA2_Stream3, DISABLE);	                                                       
	                                                                                                                 
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);                                    
	DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);                                    
}




