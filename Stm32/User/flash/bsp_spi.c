 /**
  ******************************************************************************
  * @file    bsp_spi.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash �ײ�Ӧ�ú���bsp 
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./flash/bsp_spi.h"
#include "stdio.h"

 /**
  * @brief  DMA��ʼ��
  */
//#include "./flash/bsp_spi.h"
//#include "stdio.h"
//#include "stm32f4xx_dma.h"

#define	SPI1_DMA


static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/*����һ��Ҫ>=���ݽ��շ��ʹ�С*/
uint16_t SPI_TX_BUFFER[35] =   {CONVERT0,CONVERT1,CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT32,CONVERT33,CONVERT34};
//uint16_t SPI_TX_BUFFER[1]= {0xfe00};
uint16_t SPI_TX_intan[5] = {0xe800, 0xe900, 0xea00, 0xeb00, 0xec00}; //AScii intan
	
uint16_t SPI_RX_BUFFER[1024];

 /**
  * @brief  SPI��ʼ��
  * @param  ��
  * @retval ��
  */
void SPI1_Init(u16 x)
{
	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* ʹ�� SPI ��GPIO ʱ�� */
  /*!< SPI_SPI_CS_GPIO, SPI_SPI_MOSI_GPIO, 
       SPI_SPI_MISO_GPIO,SPI_SPI_SCK_GPIO ʱ��ʹ�� */
  RCC_AHB1PeriphClockCmd (SPI_SCK_GPIO_CLK | SPI_MISO_GPIO_CLK|SPI_MOSI_GPIO_CLK|CS_GPIO_CLK, ENABLE);

  /*!< SPI_SPI ʱ��ʹ�� */
  SPI_CLK_INIT(SPI_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
  //�������Ÿ���
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT,SPI_SCK_PINSOURCE,SPI_SCK_AF); 
	GPIO_PinAFConfig(SPI_MISO_GPIO_PORT,SPI_MISO_PINSOURCE,SPI_MISO_AF); 
	GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT,SPI_MOSI_PINSOURCE,SPI_MOSI_AF); 
  
  /*!< ���� SPI_SPI ����: SCK */
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;//PB3
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  
  
  GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_SPI ����: MISO */            
  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;    //PB4
  GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_SPI ����: MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;      //PB5
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< ���� SPI_SPI ����: CS */                   //PG6
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);

  /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
  SPI_CS_HIGH();

  /* SPI ģʽ���� */
  // FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
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
	
 /* ʹ�� SPI  */
  
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
//	/*********************  DMA_Rx == ͨ��3  ������2 *********************/
//	DMA_DeInit(DMA2_Stream2);	//DMA2 ������2   
//	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//ͨ��3 SPI1_RX
//	DMA_InitStructure.DMA_BufferSize=1; //�����������Ĵ�С
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory; 	//DMA���䷽��spi����Ϊ����Դ
//	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_RX_BUFFER;	//���ݻ�������ַ
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord ; //�ڴ����ݿ�� ���ݿ��16λ
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//��������ʱ���ڴ��ַ����
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//�ڴ治ѭ���ɼ�����
//	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 �������ַ
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //�������ݿ�� ���ݿ��16λ
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//�����ַ�̶�����  ENABLEʱ�´η������ݵ�ַ����+1
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//ͨ�����ȼ�                   
//	
//	
//	DMA_Init(DMA2_Stream2,&DMA_InitStructure);		//DMA2  SPI1_RX 
//	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);

//	
////	/*********************  DMA_Tx  *********************/
////	DMA_DeInit(DMA2_Stream3);	//DMA2 ������3 SPI1_TX 
////	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//ͨ��3 SPI1_TX
////	DMA_InitStructure.DMA_BufferSize=1; //�����������Ĵ�С
////	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA���䷽��DST �ڴ���Ϊ����Դ
////	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_TX_BUFFER;	//���ݻ�������ַ
////	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //�ڴ����ݿ�� ���ݿ��16λ
////	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//��������ʱ���ڴ��ַ����
////	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//�ڴ治ѭ���ɼ�����
////	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 �������ַ
////	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //�������ݿ�� ���ݿ��16λ
////	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//�����ַ�̶�����  ENABLEʱ�´η������ݵ�ַ����+1
////	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//ͨ�����ȼ�
////	
////	DMA_Init(DMA2_Stream3,&DMA_InitStructure);		//DMA2  SPI1_TX 
////	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);//ʹ��DMA2��3�Ĵ�������ж�
//////	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;//??1????
//////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//?????3
//////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//????3
//////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ????
//////	NVIC_Init(&NVIC_InitStructure);
////	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);	//ʹ��DMA Txͨ��
////	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);	//ʹ��DMA Rxͨ��

//	/*********************  DMA_Tx  *********************/
//	DMA_DeInit(DMA2_Stream3);	//DMA2 ������3 SPI1_TX 
//	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//ͨ��3 SPI1_TX
//	DMA_InitStructure.DMA_BufferSize=1; //�����������Ĵ�С
//	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA���䷽��DST �ڴ���Ϊ����Դ
//	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)&buf;	//���ݻ�������ַ
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //�ڴ����ݿ�� ���ݿ��16λ
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//��������ʱ���ڴ��ַ����
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//�ڴ治ѭ���ɼ�����
//	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 �������ַ
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //�������ݿ�� ���ݿ��16λ
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//�����ַ�̶�����  ENABLEʱ�´η������ݵ�ַ����+1
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//ͨ�����ȼ�
////	
//	DMA_Init(DMA2_Stream3,&DMA_InitStructure);		//DMA2  SPI1_TX 
//	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);//ʹ��DMA2��3�Ĵ�������ж�
////	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;//??1????
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//?????3
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//????3
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ????
////	NVIC_Init(&NVIC_InitStructure);
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);	//ʹ��DMA Txͨ��
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);	//ʹ��DMA Rxͨ��
//}



//void spi_trans(void)    
//{
//	SPI_Cmd(SPI1, ENABLE);

//	DMA_Cmd(DMA2_Stream2, DISABLE);                                                    
//	DMA_Cmd(DMA2_Stream3, DISABLE); 

////	DMA_SetCurrDataCounter(DMA2_Stream2,NumByte);		//�趨ͨ���ڴ���
////	DMA_SetCurrDataCounter(DMA2_Stream3,NumByte);	

////  DMA2_Stream2->M0AR = (uint32_t)rx_buf;                                                     //  ???? ??????????
////	DMA2_Stream3->M0AR = (uint32_t)tx_buf;
//	
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	//�ȴ����   
//	                                                                                                            
//	DMA_Cmd(DMA2_Stream2, ENABLE);                                                           
//	DMA_Cmd(DMA2_Stream3, ENABLE);                                                        
//	                                                                                       
//	while( DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3) == RESET);   
//	while( DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET); 
//                                                                                                            
//	DMA_Cmd(DMA2_Stream2, DISABLE);                                                         
//	DMA_Cmd(DMA2_Stream3, DISABLE);	    

//    DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);		//��մ������flag
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
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
} 
/*********************************************END OF FILE**********************/
