#include "sys.h"
#include "usart.h"	
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
	while((UART4->SR&0X40)==0);//ѭ������,ֱ���������   
	UART4->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��UART5ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_UART4); //GPIOA9����ΪUART5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_UART4); //GPIOA10����ΪUART5
	
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
  USART_Init(UART4, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���1 
	
#if EN_USART1_RX	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�

	//UART5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����1�ж�ͨ��
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
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

void UART4_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
	int i;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
		{
		Res =USART_ReceiveData(UART4);//(USART1->DR);	//��ȡ���յ�������
		
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
				
		if(Res == 71)// send fixed read only register(ascii INTAN)
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;}
			
		if(Res == 112) //ָ��70������32ͨ�� + FFFFУ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;sign10=0;}
			
		if(Res == 113) //ָ��71��ѭ������35ͨ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;sign10=0;}
			
		if(Res == 114) //ָ��72��������ѹͨ�� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=1;sign9=0;sign10=0;}
			
		if(Res == 115) //ָ��73����SD�� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;sign10=0;}
				
		if(Res == 120) //ָ��78���ر� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;Usart_SendHalfWord(UART4,5555);}				
  } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

 



