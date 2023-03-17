/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ض���c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"

 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
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
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
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

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
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

/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	int i;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if(Res == 33) //ָ��21������ÿͨ���������� ��20kSamples/s;���ô��ڲ����ʣ�921600
			                                  		
			{sign1=1;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
    
		if(Res == 34) //ָ��22������ÿͨ���������� ��10kSamples/s;���ô��ڲ����ʣ�460800
		
			{sign1=0;sign2=1;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 35) //ָ��23������ÿͨ���������� ��1.25kSamples/s;���ô��ڲ����ʣ�256000 
		
			{sign1=0;sign2=0;sign3=1;sign4=0;sign5=0;sign6=0;sign7=0;} 
		
		if(Res == 36) //ָ��24������ÿͨ���������� ��300Samples/s;���ô��ڲ����ʣ�115200
		
			{sign1=0;sign2=0;sign3=0;sign4=1;sign5=0;sign6=0;sign7=0;} 
		
    if(Res == 37)//ָ��25����0ͨ��
			{sign12=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 38)//ָ��26����1ͨ��
			{sign13=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 39)//ָ��27����2ͨ��
			{sign14=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 40)//ָ��28����3ͨ��
			{sign15=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 41)//ָ��29����4ͨ��
			{sign16=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 42)//ָ��2A����5ͨ��
			{sign17=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 43)//ָ��2B����6ͨ��
			{sign18=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 44)//ָ��2C����7ͨ��
			{sign19=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 45)//ָ��2D����8ͨ��
			{sign20=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 46)//ָ��2E����9ͨ��
			{sign21=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}			
			
    if(Res == 47)//ָ��2F����10ͨ��
			{sign22=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 48)//ָ��30����11ͨ��
			{sign23=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 49)//ָ��31����12ͨ��
			{sign24=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 50)//ָ��32����13ͨ��
			{sign25=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 51)//ָ��33����14ͨ��
			{sign26=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 52)//ָ��34����15ͨ��
			{sign27=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 53)//ָ��35����16ͨ��
			{sign28=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 54)//ָ��36����17ͨ��
			{sign29=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 55)//ָ��37����18ͨ��
			{sign30=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 56)//ָ��38����19ͨ��
			{sign31=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
			
    if(Res == 57)//ָ��39����20ͨ��
			{sign32=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 58)//ָ��3A����21ͨ��
			{sign33=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 59)//ָ��3B����22ͨ��
			{sign34=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 60)//ָ��3C����23ͨ��
			{sign35=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 61)//ָ��3D����24ͨ��
			{sign36=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 62)//ָ��3E����25ͨ��
			{sign37=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 63)//ָ��3F����26ͨ��
			{sign38=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 64)//ָ��40����27ͨ��
			{sign39=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 65)//ָ��41����28ͨ��
			{sign40=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 66)//ָ��42����29ͨ��
			{sign41=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}			
			
    if(Res == 67)//ָ��43����30ͨ��
			{sign42=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}

    if(Res == 68)//ָ��44����31ͨ��
			{sign43=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
			
		if(Res == 69)//ָ��45����33ͨ��
			{sign44=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
				
		if(Res == 70)//ָ��46����34ͨ��
			{sign45=1;sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;}
				
		if(Res == 71)// send fixed read only register(ascii INTAN)
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;}
			
		if(Res == 112) //ָ��70������32ͨ�� + FFFFУ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=1;sign7=0;sign8=0;sign9=0;}
			
		if(Res == 113) //ָ��71��ѭ������35ͨ��
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=1;sign8=0;sign9=0;}
			
		if(Res == 115) //ָ��73����SD�� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=1;}
				
		if(Res == 120) //ָ��78���ر� 
		
			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;i=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=0;sign12=0;sign13=0;sign14=0;sign15=0;sign16=0;sign17=0;sign18=0;sign19=0;sign20=0;sign21=0;sign22=0;sign23=0;sign24=0;sign25=0;sign26=0;sign27=0;sign28=0;sign29=0;sign30=0;sign31=0;sign32=0;sign33=0;sign34=0;sign35=0;sign36=0;sign37=0;sign38=0;sign39=0;sign40=0;sign41=0;sign42=0;sign43=0;sign44=0;sign45=0;}				

				//		if(Res == 69||Res == 70||Res == 71||Res == 72||Res == 73||Res == 74||Res == 75||
//			 Res == 76||Res == 77||Res == 78||Res == 79||Res == 80||Res == 81||Res == 82||
//		   Res == 83||Res == 84||Res == 85||Res == 86||Res == 87||Res == 88||Res == 89||
//		   Res == 90||Res == 91||Res == 92||Res == 93||Res == 94||Res == 95||Res == 96||
//		   Res == 97||Res == 98||Res == 99||Res == 100||Res == 101) //ָ��45--65����ѡ����3��ͨ�� 
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;sign5=0;sign6=0;sign7=0;sign8=1;i=Res-69;sign9=0;}
//				
//	  if(Res == 102) //ָ��66����������ͨ��1-33
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=1;sign11=0;}
//				
//	  if(Res == 103) //ָ��67������ż��ͨ��0-34
//		
//			{sign1=0;sign2=0;sign3=0;sign4=0;i=0;sign5=0;sign6=0;sign7=0;sign8=0;sign9=0;sign10=0;sign11=1;}
  }		
}
/*********************************************END OF FILE**********************/
