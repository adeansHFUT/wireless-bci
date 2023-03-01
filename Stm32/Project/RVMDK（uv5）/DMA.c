#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "./flash/bsp_spi.h"

#define Rx_BFSize  (u16)4096
#define Tx_BFSize  (u16)4096
/*����һ��Ҫ>=���ݽ��շ��ʹ�С*/
uint16_t SPI_RX_BUFFER[Rx_BFSize];	
uint16_t SPI_TX_BUFFER[Tx_BFSize];


void SPI1_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(DMA2_CLK,ENABLE);
	
	/*********************  DMA_Rx == ͨ��3  ������2 *********************/
	DMA_DeInit(DMA2_Stream2);	//DMA2 ������2   
	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//ͨ��3 SPI1_RX
	DMA_InitStructure.DMA_BufferSize=16; //�����������Ĵ�С
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory; 	//DMA���䷽��spi����Ϊ����Դ
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_RX_BUFFER;	//���ݻ�������ַ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord ; //�ڴ����ݿ�� ���ݿ��16λ
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//��������ʱ���ڴ��ַ����
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//�ڴ治ѭ���ɼ�����
	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 �������ַ
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //�������ݿ�� ���ݿ��16λ
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//�����ַ�̶�����  ENABLEʱ�´η������ݵ�ַ����+1
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//ͨ�����ȼ�                   
	
	DMA_Init(DMA2_Stream2,&DMA_InitStructure);		//DMA2  SPI1_RX 
	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC | DMA_IT_TE, ENABLE);
  // �жϳ�ʼ�� 
  /* ���� DMA����Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;  	
  /* �������ȼ� */  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		
  /* ��Ӧ���ȼ� */  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				
  /* ʹ���ⲿ�ж�ͨ�� */  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ����NVIC */	
  NVIC_Init(&NVIC_InitStructure);

	
	
	/*********************  DMA_Tx  *********************/
	DMA_DeInit(DMA2_Stream3);	//DMA2 ������3 SPI1_TX 
	DMA_InitStructure.DMA_Channel = DMA_Channel_3 ;//ͨ��3 SPI1_TX
	DMA_InitStructure.DMA_BufferSize=16; //�����������Ĵ�С
	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral; 	//DMA���䷽��DST �ڴ���Ϊ����Դ
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)SPI_TX_BUFFER;	//���ݻ�������ַ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //�ڴ����ݿ�� ���ݿ��16λ
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  		//��������ʱ���ڴ��ַ����
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; 		//�ڴ治ѭ���ɼ�����
	DMA_InitStructure.DMA_PeripheralBaseAddr=SPI1_DR_Addr;	//SPI1 �������ַ
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;  //�������ݿ�� ���ݿ��16λ
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 	//�����ַ�̶�����  ENABLEʱ�´η������ݵ�ַ����+1
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;		//ͨ�����ȼ�
	
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
    // DMA �������
    if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))	
    {
        // ���DMA������ɱ�־
        DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);	
        // Ƭѡ���ߣ����ݷ������	
        SPI_CS_HIGH();	
    }
}

void DMA2_Stream2_IRQHandler(void)
{				
    // DMA�������
    if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))	
    {		
        // ���ݽ������ ����Ƭѡ
        SPI_CS_HIGH();	
        // ���DMA������ɱ�־λ		
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);	
    }
}


void spi_trans(u16 *rx_buf,u16 *tx_buf,u16 NumByte)    
{
  SPI_Cmd(SPI1, ENABLE);
	DMA_Cmd(DMA2_Stream2, DISABLE);                                                    
	DMA_Cmd(DMA2_Stream3, DISABLE); 

	DMA_SetCurrDataCounter(DMA2_Stream2,NumByte);		//�趨ͨ���ڴ���
	DMA_SetCurrDataCounter(DMA2_Stream3,NumByte);	

  DMA2_Stream2->M0AR = (uint32_t)rx_buf;                                                     //  ???? ??????????
	DMA2_Stream3->M0AR = (uint32_t)tx_buf;

  SPI1->DR;	
                                                                                                                                                                                                                                                           
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	//�ȴ����   
	                                                                                                            
	DMA_Cmd(DMA2_Stream2, ENABLE);                                                           
	DMA_Cmd(DMA2_Stream3, ENABLE);                                                        
	                                                                                       
	while( DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3) == RESET);   
	while( DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET);    
	                                                                                                            
	DMA_Cmd(DMA2_Stream2, DISABLE);                                                         
	DMA_Cmd(DMA2_Stream3, DISABLE);	                                                       
	                                                                                                                 
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);                                    
	DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);                                    
}




