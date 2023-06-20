#include "spi.h"
#include "delay.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
uint16_t SPI_TX_BUFFER[35] =   {CONVERT0,CONVERT1,CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT32,CONVERT33,CONVERT34};
uint16_t SPI_TX_BUFFER_2[32] = {CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT0,CONVERT1};
uint16_t SPI_TX_intan[5] = {0xe800, 0xe900, 0xea00, 0xeb00, 0xec00}; //AScii intan
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(u16 x)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOBʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PB3����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PB4����Ϊ SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB5����Ϊ SPI1
  //GPIOFB3,4,5��ʼ������
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;//PB3~5���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PB3~5���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
		/*!< ���� SPI_SPI ����: CS */                 
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);

  /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
  SPI_CS_HIGH();
	

 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	//SPI_InitStructure.SPI_BaudRatePrescaler = x;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

  delay_ms(1000);

/* ��ʼ��оƬ���üĴ��� */
	
	   SPI_CS_LOW(); delay_us( 50 );
	 
	   SPI_SendHalfWord(0x80fe);
	
     delay_us( 50 );SPI_CS_HIGH();
		
	   delay_us( 84000 );
	 
	 /* ��ʼ��оƬ���üĴ���0��ADC���úͷŴ������ٽ��� */
	 	SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x80de);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���1��������������ADC������ƫ�õ������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8120);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		/* ��ʼ��оƬ���üĴ���2��������������ADC������ƫ�õ������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8228);

		delay_us( 50 );SPI_CS_HIGH();
	
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���3���¶ȴ������͸�������������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8301);
	
		delay_us( 50 );SPI_CS_HIGH();
		
    delay_us( 500 );
	
	 	/* ��ʼ��оƬ���üĴ���4��ADC�����ʽ��DSPʧ���������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8480);

		delay_us( 50 );SPI_CS_HIGH();
		
    delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���5���迹�����ƣ�δ���������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8500);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���6���Ĵ���5��DAC���ã����㣩���� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8600);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���7��ͬ���������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8700);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���8���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x881e);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���9���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���10���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8a2b);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���11���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8b86);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );

	 	/* ��ʼ��оƬ���üĴ���12���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8c10);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );	
						
	 	/* ��ʼ��оƬ���üĴ���13���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8dfc);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���14���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8eff);
    
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���15���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8fff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���16���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x90ff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���17���Ŵ����������� */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x91ff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		/* �Ĵ�����ʼ����100us����ADCУ׼,У׼��ʼ����Ҫ����9����ʱ�� */
    delay_us( 20000 );
		
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x5500);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff0);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );

		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff1);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff2);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff3);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff4);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff5);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff6);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff7);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff8);
		
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
		SPI_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(0xfff9);
		
		delay_us( 50 );SPI_CS_HIGH();
		
    delay_us( 5000 );	
}   
 
//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u16 SPI_SendHalfWord(u16 HalfWord)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, HalfWord); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
 		    
}








