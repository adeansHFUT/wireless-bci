#include "sys.h"
#include "usart.h"	
#include "string.h"
#define BUFFER_SIZE 64
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4探索者开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/6/10
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
	USART6->DR = (u8) ch;      
	return ch;
}
#endif
 
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
uint8_t tmpbuf_rev[BLOCK_DMA_NUM_TX][BLOCK_DMA_SIZE_TX]; // static mem for dma send
char rx_buffer[BUFFER_SIZE_RX];  // dma receive
volatile uint32_t rx_index = 0;  // dma receive size  


void USART6_DMA_Tx_Configuration(void)  //  DMA Tx configuration
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);				//DMA1????
	DMA_DeInit(DMA2_Stream7);
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);						//??DMA???
	DMA_InitStructure.DMA_Channel = DMA_Channel_5; 							//DMA????
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART6->DR);		//DMA????
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&tmpbuf_rev[0][0];	//??????
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					//DMA????:??--->??
	DMA_InitStructure.DMA_BufferSize = BLOCK_DMA_SIZE_TX;		//????????
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//???????
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//???????
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//set memory data size = 8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//set memory data size = 16bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//?????? 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//????? DMA_Priority_High
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//?????????
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//????????
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);								//???DMA Stream
	DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_FEIF7);
	
	DMA_Cmd(DMA2_Stream7, DISABLE); 										//??DMA??


}
void USART6_DMA_Rx_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);			//DMA1????
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);						//??DMA???  
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  						//????
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;		//DMA????
	memset(rx_buffer, 0, BUFFER_SIZE_RX); // set to zero
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
	while (DMA_GetCmdStatus(DMA2_Stream1) == DISABLE);
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
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == SET);	//???????????????????(??:?????USART_Mode_Tx,???????????)
	USART_ClearFlag(USART6, USART_FLAG_TC);	//????????
 
  USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);				//??USART1???????
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);				//??USART1?????
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);				//??USART1???? 
	USART_ITConfig(USART6, USART_IT_TC, DISABLE);				//??USART1?????? 
	
	USART_DMACmd(USART6 ,   USART_DMAReq_Tx,ENABLE);  			//?????DMA??
	USART_DMACmd(USART6 ,   USART_DMAReq_Rx,ENABLE);  			//?????DMA??
 
}


u8 DMA_send_data(const uint8_t* data, uint32_t length) {
	 
		static uint8_t first_call = 1;

		if(first_call == 0 && DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) == RESET)  // if status is 0, transfer is not complte
				return 0;// return error because last DMA transfer is not finish yet
		

		// ??DMA2_Stream7????
		//DMA_Cmd(DMA2_Stream7, DISABLE);
		while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
    DMA_MemoryTargetConfig(DMA2_Stream7, (uint32_t)data, DMA_Memory_0);
		DMA_SetCurrDataCounter(DMA2_Stream7, length);
		DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);   // maybe no need
		DMA_Cmd(DMA2_Stream7, ENABLE); 
		while (DMA_GetCmdStatus(DMA2_Stream7) == DISABLE);
		first_call = 0;
		return 1;
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
		if (ch > 0) // handle string in rx_buffer
		{
			if(strcmp(rx_buffer, "S_i") == 0) // send INTAN
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;sign11=0;sign12=0;}
					
			else if(strcmp(rx_buffer, "C_+") == 0)	// control delay 
			  {intan_cs_delay++;}
					
			else if(strcmp(rx_buffer, "C_-") == 0)	// control delay 
			  {intan_cs_delay--;}
				
			else if(strcmp(rx_buffer, "S_dma32") == 0)	// send 32 by dma
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;sign12=1;}	
					
			else if(strcmp(rx_buffer, "S_c") == 0)	// send constant
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=1;sign12=0;}	
				
			else if(strcmp(rx_buffer, "S_32") == 0) // send 32 channels
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;sign10=0;sign11=0;sign12=0;}
					
			else if(strcmp(rx_buffer, "store") == 0) // store sd
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;sign10=0;sign11=0;sign12=0;}
					
			else if(strcmp(rx_buffer, "pause") == 0) // pause
				{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;sign12=0;}		
					
			memset(rx_buffer, 0, BUFFER_SIZE_RX); // set to zero
		}
		DMA_SetCurrDataCounter(DMA2_Stream1 , BUFFER_SIZE_RX);
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // enable DMA
	}
}


//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能UART6时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_USART6); //GPIOA9复用为UART5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_USART6); //GPIOA10复用为UART5
	
	//UART5端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART6, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART6, ENABLE);  //使能串口1 
	
#if EN_USART1_RX	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断

	//UART5 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0X00FF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
void USART6_IRQHandler(void)                	//串口4中断服务程序
{
	u8 Res;
	int i;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //接收中断
		{
		Res =USART_ReceiveData(USART6);//(USART1->DR);	//读取接收到的数据
		
		if(Res == 33) //指令21：设置每通道采样速率 ：20kSamples/s
			                                  		
			{sign1=1;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
    
		if(Res == 34) //指令22：设置每通道采样速率 ：10kSamples/s
		
			{sign1=0;sign2=1;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 35) //指令23：设置每通道采样速率 ：1.25kSamples/s 
		
			{sign1=0;sign2=0;sign3=1;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 36) //指令24：设置每通道采样速率 ：150Samples/s
		
			{sign1=0;sign2=0;sign3=0;sign4=1;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 37||Res == 38||Res == 39||Res == 40||Res == 41||Res == 42||Res == 43||
			 Res == 44||Res == 45||Res == 46||Res == 47||Res == 48||Res == 49||Res == 50||
		   Res == 51||Res == 52||Res == 53||Res == 54||Res == 55||Res == 56||Res == 57||
		   Res == 58||Res == 59||Res == 60||Res == 61||Res == 62||Res == 63||Res == 64||
		   Res == 65||Res == 66||Res == 67||Res == 68) //指令25...29,2A....2F...44：任选32通道之一 
		
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
			
		if(Res == 112) //指令70：采样32通道 + FFFF校验
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 113) //指令71：循环采样35通道
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 114) //指令72：采样电压通道 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=1;sign9=0;sign10=0;sign11=0;}
			
		if(Res == 115) //指令73：存SD卡 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;sign10=0;sign11=0;}
				
		if(Res == 120) //指令78：关闭 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;}				
  } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
*/
 



