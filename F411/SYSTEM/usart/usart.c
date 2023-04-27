#include "sys.h"
#include "usart.h"	
#include "string.h"
#define BUFFER_SIZE 64
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F4̽���߿�����
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/6/10
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
	USART6->DR = (u8) ch;      
	return ch;
}
#endif
 
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	
uint16_t tmpbuf_rev[BLOCK_DMA_NUM_TX][BLOCK_DMA_SIZE_TX]; // static mem for dma send
uint8_t rx_buffer[BUFFER_SIZE_RX];  // dma receive
volatile uint32_t rx_index = 0;  // dma receive size  


void USART6_DMA_Tx_Configuration(void)  //  DMA Tx configuration
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);				//DMA1????
	DMA_DeInit(DMA2_Stream6);
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);						//??DMA???
	DMA_InitStructure.DMA_Channel = DMA_Channel_5; 							//DMA????
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;		//DMA????
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&tmpbuf_rev[0][0];	//??????
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					//DMA????:??--->??
	DMA_InitStructure.DMA_BufferSize = BLOCK_DMA_SIZE_TX;		//????????
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//???????
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//???????
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//set memory data size = 8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//set memory data size = 16bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//?????? 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//????? DMA_Priority_High
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//?????????
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//????????
	DMA_Init(DMA2_Stream6, &DMA_InitStructure);								//???DMA Stream
	DMA_Cmd(DMA2_Stream6, DISABLE); 										//??DMA??

}
void USART6_DMA_Rx_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);			//DMA1????
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);						//??DMA???  
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  						//????
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;		//DMA????
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx_buffer;	//??????
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;				//DMA????:????????:??--->??
	DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE_RX;		//???? 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//???????
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//???????
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//??????:8?
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//???????:8?
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//?????? 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//????? DMA_Priority_VeryHigh
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//?????????
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//????????
	DMA_Init(DMA2_Stream1 , &DMA_InitStructure);							//???DMA_Stream	
	DMA_Cmd(DMA2_Stream1, ENABLE);  										//??DMA??
}
void USART6_Configuration(uint16_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART6); // USART6 RX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART6); // USART6 TX
 
 // Configure USART6 pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	
	/* Enable the USART Interrupt */
 
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;     // ??????
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART6, ENABLE);
	
 // config usart IT
	USART_ClearFlag(USART6, USART_FLAG_TC); //????????	
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);	//???????????????????(??:?????USART_Mode_Tx,???????????)
	USART_ClearFlag(USART6, USART_FLAG_TC);	//????????
 
  USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);				//??USART1???????
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);				//??USART1?????
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);				//??USART1???? 
	USART_ITConfig(USART6, USART_IT_TC, ENABLE);				//??USART1?????? 
	
	USART_DMACmd(USART6 ,   USART_DMAReq_Tx,ENABLE);  			//?????DMA??
	USART_DMACmd(USART6 ,   USART_DMAReq_Rx,ENABLE);  			//?????DMA??
 
}


void DMA_send_data(const uint16_t* data, uint32_t length) {

		if(DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6) == RESET)
				return;// return error
		

		// ??DMA2_Stream6????
		DMA_Cmd(DMA2_Stream6, DISABLE);
		DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_FEIF6);   // maybe no need
    DMA_MemoryTargetConfig(DMA2_Stream6, (uint32_t)data, DMA_Memory_0);
		DMA_SetCurrDataCounter(DMA2_Stream6, length);
		DMA_Cmd(DMA2_Stream6, ENABLE);
}

void USART6_IRQHandler(void)
{
	int16_t ch;
	
	if (USART_GetITStatus(USART6,USART_IT_IDLE) != RESET)
	{		
		USART_ClearITPendingBit(USART6 , USART_IT_IDLE);	//?????????????,??????????,DMA??????(??SR,???DR??????????)??:?????,?????????????
		ch =  USART_ReceiveData(USART6);					//?????????????,??????????,DMA??????(??SR,???DR??????????)??:?????,?????????????
		
		DMA_Cmd(DMA2_Stream1 , DISABLE); 					// disable DMA
		DMA_ClearFlag(DMA2_Stream1 , DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);//?????
		ch = BUFFER_SIZE_RX - DMA_GetCurrDataCounter(DMA2_Stream1);
		if (ch > 0)
		{
			// handle 
			memset(rx_buffer, 0, BUFFER_SIZE_RX); // set to zero
		}
		DMA_SetCurrDataCounter(DMA2_Stream1 , BUFFER_SIZE_RX);
		DMA_Cmd(DMA2_Stream1, ENABLE);  // enable DMA
	}
}


//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��UART6ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_USART6); //GPIOA9����ΪUART5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_USART6); //GPIOA10����ΪUART5
	
	//UART5�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
#if EN_USART1_RX	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�

	//UART5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0X00FF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
void USART6_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
	int i;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�
		{
		Res =USART_ReceiveData(USART6);//(USART1->DR);	//��ȡ���յ�������
		
		if(Res == 33) //ָ��21������ÿͨ���������� ��20kSamples/s
			                                  		
			{sign1=1;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
    
		if(Res == 34) //ָ��22������ÿͨ���������� ��10kSamples/s
		
			{sign1=0;sign2=1;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 35) //ָ��23������ÿͨ���������� ��1.25kSamples/s 
		
			{sign1=0;sign2=0;sign3=1;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 36) //ָ��24������ÿͨ���������� ��150Samples/s
		
			{sign1=0;sign2=0;sign3=0;sign4=1;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 37||Res == 38||Res == 39||Res == 40||Res == 41||Res == 42||Res == 43||
			 Res == 44||Res == 45||Res == 46||Res == 47||Res == 48||Res == 49||Res == 50||
		   Res == 51||Res == 52||Res == 53||Res == 54||Res == 55||Res == 56||Res == 57||
		   Res == 58||Res == 59||Res == 60||Res == 61||Res == 62||Res == 63||Res == 64||
		   Res == 65||Res == 66||Res == 67||Res == 68) //ָ��25...29,2A....2F...44����ѡ32ͨ��֮һ 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=Res-37;sign5=1;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;} 
		
		if(Res == 71)// hex:47 send fixed read only register(ascii INTAN)
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;sign11=0;}
		
		if(Res == 72)// send constant
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=1;}
				
		if(Res == 73)// cs delay++
			{intan_cs_delay++;}
		
		if(Res == 74)// cs delay--
			{intan_cs_delay--;}

		if(Res == 75)
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;sign12=1;}
			
		if(Res == 112) //ָ��70������32ͨ�� + FFFFУ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 113) //ָ��71��ѭ������35ͨ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 114) //ָ��72��������ѹͨ�� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=1;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 115) //ָ��73����SD�� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;sign10=0;sign11=0;}
				
		if(Res == 120) //ָ��78���ر� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;}				
  } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
*/
 



