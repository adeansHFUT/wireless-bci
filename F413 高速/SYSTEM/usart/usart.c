#include "sys.h"
#include "usart.h"	
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
	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
	UART4->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能UART5时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_UART4); //GPIOA9复用为UART5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_UART4); //GPIOA10复用为UART5
	
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
  USART_Init(UART4, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(UART4, ENABLE);  //使能串口1 
	
#if EN_USART1_RX	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启相关中断

	//UART5 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口1中断通道
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
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

void UART4_IRQHandler(void)                	//串口4中断服务程序
{
	u8 Res;
	int i;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
		{
		Res =USART_ReceiveData(UART4);//(USART1->DR);	//读取接收到的数据
		
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
				
		if(Res == 71)// send fixed read only register(ascii INTAN)
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;}
			
		if(Res == 112) //指令70：采样32通道 + FFFF校验
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;sign10=0;}
			
		if(Res == 113) //指令71：循环采样35通道
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;sign10=0;}
			
		if(Res == 114) //指令72：采样电压通道 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=1;sign9=0;sign10=0;}
			
		if(Res == 115) //指令73：存SD卡 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;sign10=0;}
				
		if(Res == 120) //指令78：关闭 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;Usart_SendHalfWord(UART4,5555);}				
  } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	

 



