/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"

 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
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
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
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

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
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

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	int i;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if(Res == 33) //指令21：设置每通道采样速率 ：20kSamples/s;设置串口波特率：921600
			                                  		
			{sign1=1;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
    
		if(Res == 34) //指令22：设置每通道采样速率 ：10kSamples/s;设置串口波特率：460800
		
			{sign1=0;sign2=1;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 35) //指令23：设置每通道采样速率 ：1.25kSamples/s;设置串口波特率：256000 
		
			{sign1=0;sign2=0;sign3=1;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 36) //指令24：设置每通道采样速率 ：300Samples/s;设置串口波特率：115200
		
			{sign1=0;sign2=0;sign3=0;sign4=1;sign5=0;sign6=0;sign7=0;} 
		
    if(Res == 37)//指令25：打开0通道
			{sign12=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 38)//指令26：打开1通道
			{sign13=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 39)//指令27：打开2通道
			{sign14=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 40)//指令28：打开3通道
			{sign15=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 41)//指令29：打开4通道
			{sign16=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 42)//指令2A：打开5通道
			{sign17=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 43)//指令2B：打开6通道
			{sign18=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 44)//指令2C：打开7通道
			{sign19=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 45)//指令2D：打开8通道
			{sign20=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 46)//指令2E：打开9通道
			{sign21=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}			
			
    if(Res == 47)//指令2F：打开10通道
			{sign22=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 48)//指令30：打开11通道
			{sign23=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 49)//指令31：打开12通道
			{sign24=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 50)//指令32：打开13通道
			{sign25=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 51)//指令33：打开14通道
			{sign26=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 52)//指令34：打开15通道
			{sign27=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 53)//指令35：打开16通道
			{sign28=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 54)//指令36：打开17通道
			{sign29=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 55)//指令37：打开18通道
			{sign30=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 56)//指令38：打开19通道
			{sign31=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
			
    if(Res == 57)//指令39：打开20通道
			{sign32=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 58)//指令3A：打开21通道
			{sign33=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 59)//指令3B：打开22通道
			{sign34=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 60)//指令3C：打开23通道
			{sign35=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 61)//指令3D：打开24通道
			{sign36=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 62)//指令3E：打开25通道
			{sign37=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 63)//指令3F：打开26通道
			{sign38=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 64)//指令40：打开27通道
			{sign39=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 65)//指令41：打开28通道
			{sign40=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 66)//指令42：打开29通道
			{sign41=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}			
			
    if(Res == 67)//指令43：打开30通道
			{sign42=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 68)//指令44：打开31通道
			{sign43=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
			
		if(Res == 69)//指令45：打开33通道
			{sign44=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
				
		if(Res == 70)//指令46：打开34通道
			{sign45=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
				
		if(Res == 71)// send fixed read only register(ascii INTAN)
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;}
			
		if(Res == 112) //指令70：采样32通道 + FFFF校验
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;}
			
		if(Res == 113) //指令71：循环采样35通道
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;}
			
		if(Res == 115) //指令73：存SD卡 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;}
				
		if(Res == 120) //指令78：关闭 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;sign12=0;sign13=0;sign14=0;sign15=0;sign16=0;sign17=0;sign18=0;sign19=0;sign20=0;sign21=0;sign22=0;sign23=0;sign24=0;sign25=0;sign26=0;sign27=0;sign28=0;sign29=0;sign30=0;sign31=0;sign32=0;sign33=0;sign34=0;sign35=0;sign36=0;sign37=0;sign38=0;sign39=0;sign40=0;sign41=0;sign42=0;sign43=0;sign44=0;sign45=0;}				

				//		if(Res == 69||Res == 70||Res == 71||Res == 72||Res == 73||Res == 74||Res == 75||
//			 Res == 76||Res == 77||Res == 78||Res == 79||Res == 80||Res == 81||Res == 82||
//		   Res == 83||Res == 84||Res == 85||Res == 86||Res == 87||Res == 88||Res == 89||
//		   Res == 90||Res == 91||Res == 92||Res == 93||Res == 94||Res == 95||Res == 96||
//		   Res == 97||Res == 98||Res == 99||Res == 100||Res == 101) //指令45--65：任选相邻3个通道 
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=1;i=Res-69;sign9=0;}
//				
//	  if(Res == 102) //指令66：采样奇数通道1-33
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;sign11=0;}
//				
//	  if(Res == 103) //指令67：采样偶数通道0-34
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=1;}
  }		
}
/*********************************************END OF FILE**********************/
