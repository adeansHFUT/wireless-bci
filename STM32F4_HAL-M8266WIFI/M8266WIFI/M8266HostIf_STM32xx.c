/********************************************************************
 * M8266HostIf.c
 * .Description
 *     Source file of M8266WIFI Host Interface 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"	
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"

/***********************************************************************************
 * M8266HostIf_GPIO_SPInCS_nRESET_Pin_Init                                         *
 * Description                                                                     *
 *    To initialise the GPIOs for SPI nCS and nRESET output for M8266WIFI module   *
 *    You may update the macros of GPIO PINs usages for nRESET from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_GPIO_CS_RESET_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

	//Initial STM32's GPIO for M8266WIFI_SPI_nCS
  M8266WIFI_SPI_nCS_GPIO_RCC_CLOCK_EN;  														// enable nCS GPIO clock
	GPIO_OUTPUT_HIGH(M8266WIFI_SPI_nCS_GPIO,M8266WIFI_SPI_nCS_PIN);		// nCS output high initially	
	GPIO_InitStructure_AS_GPIO_OUTPUT(M8266WIFI_SPI_nCS_GPIO,M8266WIFI_SPI_nCS_PIN);
	
	//Initial STM32's GPIO for M8266WIFI_nRESET
  M8266WIFI_nRESET_GPIO_RCC_CLOCK_EN;  															// enable nRESET GPIO clock
	GPIO_OUTPUT_HIGH(M8266WIFI_nRESET_GPIO,M8266WIFI_nRESET_PIN);		  // nRESET output high initially	
	GPIO_InitStructure_AS_GPIO_OUTPUT(M8266WIFI_nRESET_GPIO,M8266WIFI_nRESET_PIN);
}
/***********************************************************************************
 * M8266HostIf_SPI_Init                                                            *
 * Description                                                                     *
 *    To initialise the SPI Interface for M8266WIFI module                         *
 *    You may update the macros of SPI usages for nRESET from brd_cfg.h            *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
SPI_HandleTypeDef hspi;
void M8266HostIf_SPI_Init(void)
{
#if defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32L4XX)
  GPIO_InitTypeDef  GPIO_InitStructure;
#if (M8266WIFI_SPI_INTERFACE_NO == 1)
	#if 0 // if use GPIOA5/6/7 for SPI1 SCK/MISO/MOSI
	__HAL_RCC_GPIOA_CLK_ENABLE();					// Enable the GPIOE Clock
	GPIO_InitStructure.Pin       = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);  
	#elif 1 // if use GPIOB3/4/5 for SPI1 SCK/MISO/MOSI
	__HAL_RCC_GPIOB_CLK_ENABLE();					// Enable the GPIOB Clock
	GPIO_InitStructure.Pin       = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  
	#else
	#error You should specify the GPIO pins used on STM32F7 SPI1 in function M8266HostIf_SPI_Init() fromf the file M8266HostIf_STM32Fxx.c
	#endif
	__HAL_RCC_SPI1_CLK_ENABLE();
	hspi.Instance = SPI1;	
#endif

  hspi.Init.Mode 									= SPI_MODE_MASTER;
  hspi.Init.Direction 						= SPI_DIRECTION_2LINES;
  hspi.Init.DataSize 							= SPI_DATASIZE_8BIT;
  hspi.Init.CLKPolarity 					= SPI_POLARITY_LOW;
  hspi.Init.CLKPhase 							= SPI_PHASE_1EDGE;
  hspi.Init.NSS 									= SPI_NSS_SOFT;
  hspi.Init.BaudRatePrescaler 		= SPI_BAUDRATEPRESCALER_8; 
  hspi.Init.FirstBit 							= SPI_FIRSTBIT_MSB;
  hspi.Init.TIMode 								= SPI_TIMODE_DISABLE;
  hspi.Init.CRCCalculation 				= SPI_CRCCALCULATION_DISABLE;
  hspi.Init.CRCPolynomial 				= 7;
  if (HAL_SPI_Init(&hspi) != HAL_OK)
  {
    while(1);
  }
  __HAL_SPI_ENABLE(&hspi); 
	
#endif
	
}
/***********************************************************************************
 * M8266HostIf_SPI_SetSpeed                                                        *
 * Description                                                                     *
 *    To setup the SPI Clock Speed for M8266WIFI module                            *
 * Parameter(s):                                                                   *
 *    SPI_BaudRatePrescaler: SPI BaudRate Prescaler                                *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler)
{
#if  defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F3XX) || defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX)	
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	M8266WIFI_INTERFACE_SPI->CR1&=0XFFC7;
	M8266WIFI_INTERFACE_SPI->CR1|=SPI_BaudRatePrescaler;
	SPI_Cmd(M8266WIFI_INTERFACE_SPI,ENABLE);
#elif defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX)
	__HAL_SPI_DISABLE(&hspi);
  hspi.Instance->CR1&=0XFFC7;
  hspi.Instance->CR1|=SPI_BaudRatePrescaler;
  __HAL_SPI_ENABLE(&hspi);

#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif	
} 

/***********************************************************************************
 * M8266HostIf_Init                                                                *
 * Description                                                                     *
 *    To initialise the Host interface for M8266WIFI module                        *
 * Parameter(s):                                                                   *
 *    baud: baud rate to set                                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/ 
void M8266HostIf_Init(void)
{
	 M8266HostIf_GPIO_CS_RESET_Init();
	
	 M8266HostIf_SPI_Init();
//	 M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);
	
}
//�����ʼ��SPI2
u8 test1[2]={0x33,0x56};
SPI_HandleTypeDef hspi2; 
void SPI2_Init(u32 x)
{	 
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE(); // ʹ��GPIOBʱ��
  __HAL_RCC_SPI2_CLK_ENABLE(); // ʹ��SPI2ʱ��
  // ����PB13, PB14, PB15����ΪSPI2  SCK/MISO/MOSI
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; //�����������
  GPIO_InitStruct.Pull = GPIO_PULLUP; //����
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; //�ǳ��ߣ����200MHz
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	SPI2_CS_HIGH();
  // ����CS����
  GPIO_InitStruct.Pin = SPI2_CS_PIN;     //�ϱ߻ᶨ��SPI2_CS_PIN������PB12
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI2_CS_GPIO_PORT, &GPIO_InitStruct);

	__HAL_RCC_SPI2_FORCE_RESET(); // ��λ SPI1
  __HAL_RCC_SPI2_RELEASE_RESET(); // ֹͣ��λ SPI1

  // ����SPI
  hspi2.Instance = SPI2;  // �޸�Ϊ hspi2
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;//����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ;����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = x;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    while(1);
  }
  __HAL_SPI_ENABLE(&hspi2);   // �޸�Ϊ hspi2
	

  delay_ms(1000);

	/* ��ʼ��оƬ���üĴ��� */
	
	   SPI2_CS_LOW(); delay_us( 50 );
	 
	   SPI_SendHalfWord(&hspi2,0x80fe);//0x80fe��ʼ��
	
     delay_us( 50 );SPI2_CS_HIGH();
		
	   delay_us( 84000 );
	 
	 /* ��ʼ��оƬ���üĴ���0��ADC���úͷŴ������ٽ��� */
	 	SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x80de);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���1��������������ADC������ƫ�õ������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8120);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		/* ��ʼ��оƬ���üĴ���2��������������ADC������ƫ�õ������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8228);

		delay_us( 50 );SPI2_CS_HIGH();
	
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���3���¶ȴ������͸�������������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8301);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
    delay_us( 500 );
	
	 	/* ��ʼ��оƬ���üĴ���4��ADC�����ʽ��DSPʧ���������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8480);

		delay_us( 50 );SPI2_CS_HIGH();
		
    delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���5���迹�����ƣ�δ���������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8500);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���6���Ĵ���5��DAC���ã����㣩���� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8600);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���7��ͬ���������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8700);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���8���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x881e);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���9���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8985);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8985);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���10���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8a2b);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���11���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8b86);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );

	 	/* ��ʼ��оƬ���üĴ���12���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8c10);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );	
						
	 	/* ��ʼ��оƬ���üĴ���13���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8dfc);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���14���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8eff);
    
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
	 	/* ��ʼ��оƬ���üĴ���15���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x8fff);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���16���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x90ff);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* ��ʼ��оƬ���üĴ���17���Ŵ����������� */
	 
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x91ff);
	
		delay_us( 50 );SPI2_CS_HIGH();
		
		/* �Ĵ�����ʼ����100us����ADCУ׼,У׼��ʼ����Ҫ����9����ʱ�� */
    delay_us( 20000 );
		
	  SPI2_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(&hspi2,0x5500);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff0);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		

		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff1);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff2);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff3);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff4);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff5);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff6);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff7);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff8);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
		delay_us( 500 );
		
		
		SPI2_CS_LOW(); delay_us( 50 );
		
		SPI_SendHalfWord(&hspi2,0xfff9);
		
		delay_us( 50 );SPI2_CS_HIGH();
		
    delay_us( 5000 );	
}   
 
//SPI1 ��дһ���ֽ�
//����ֵ:��ȡ�����ֽ�
//Intan���յ�������

uint16_t SPI_SendHalfWord(SPI_HandleTypeDef *hspi2, uint16_t HalfWord)
{
  uint16_t receiveData;
  HAL_SPI_TransmitReceive(hspi2, (uint8_t*)&HalfWord, (uint8_t*)&receiveData, 2, 1000);  //��֤��Ҳ���淢����
  return receiveData;
}
//SPI_TX_BUFFER��SPI_TX_intan���ֱ����ڴ洢���͸�IntanоƬ��ת������ͷ��͸�IntanоƬ��ASCII�롣
//SPI_TX_BUFFER�����д洢��35��16λ��ת�������CONVERT0��CONVERT34���������ڸ���IntanоƬ��Щͨ����Ҫ��������ת����
//SPI_TX_intan�����д洢��5��16λ��ASCII�룬��0xe800��0xec00������������IntanоƬ����һЩ������������ȡ���ݡ����ò����ʵȡ�
//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTIONS ARE REQUIRED BY M8266WIFIDRV.LIB. 
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE
//////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266HostIf_Set_nRESET_Pin                                                      *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI nRESET                *
 *    You may update the macros of GPIO PIN usages for nRESET from brd_cfg.h       *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to nRESET pin                                         *
 *              0 = output LOW  onto nRESET                                        *
 *              1 = output HIGH onto nRESET                                        *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_nRESET_Pin(u8 level)
{
	  if(level!=0)
		//GPIO_SetBits(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32H7XX)
		  M8266WIFI_nRESET_GPIO->BSRRL = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F3XX) || defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif

		else
		//GPIO_ResetBits(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_nRESET_GPIO->BRR   = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32H7XX)
		  M8266WIFI_nRESET_GPIO->BSRRH = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F3XX) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN<<16;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif
}
/***********************************************************************************
 * M8266HostIf_Set_SPI_nCS_PIN                                                     *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI SPI nCS               *
 *    You may update the macros of GPIO PIN usages for SPI nCS from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to SPI nCS pin                                        *
 *              0 = output LOW  onto SPI nCS                                       *
 *              1 = output HIGH onto SPI nCS                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_SPI_nCS_Pin(u8 level)
{
	  if(level!=0)
		//GPIO_SetBits(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32H7XX)
		  M8266WIFI_SPI_nCS_GPIO->BSRRL = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F3XX) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX) 
		  M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif

		else
		//GPIO_ResetBits(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_SPI_nCS_GPIO->BRR   = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32H7XX)
		  M8266WIFI_SPI_nCS_GPIO->BSRRH = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F3XX) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX)
      M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN<<16;		
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif
}

/***********************************************************************************
 * M8266WIFIHostIf_delay_us                                                        *
 * Description                                                                     *
 *    To loop delay some micro seconds.                                            *
 * Parameter(s):                                                                   *
 *    1. nus: the micro seconds to delay                                           *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266HostIf_delay_us(u8 nus)
{
   delay_us(nus);
}

/***********************************************************************************
 * M8266HostIf_SPI_ReadWriteByte                                                   *
 * Description                                                                     *
 *    To write a byte onto the SPI bus from MCU MOSI to the M8266WIFI module       *
 *    and read back a byte from the SPI bus MISO meanwhile                         *
 *    You may update the macros of SPI usage from brd_cfg.h                        *
 * Parameter(s):                                                                   *
 *    1. TxdByte: the byte to be sent over MOSI                                    *
 * Return:                                                                         *
 *    1. The byte read back from MOSI meanwhile                                    *                                                                         *
 ***********************************************************************************/
u8 M8266HostIf_SPI_ReadWriteByte(u8 TxdByte)
{
#if  defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F2XX) || defined(MCU_IS_STM32F4XX)	
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_TXE) == RESET){} 	//wait SPI TXD Buffer Empty
	SPI_I2S_SendData(M8266WIFI_INTERFACE_SPI, TxdByte); 			//Write the byte to the TXD buffer and then shift out via MOSI
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_RXNE) == RESET){} 	//wait the SPI RXD buffer has received the data from MISO
	return SPI_I2S_ReceiveData(M8266WIFI_INTERFACE_SPI);  		//return the data received from MISO
#elif defined(MCU_IS_STM32F3XX)	
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_TXE) == RESET){} 	//wait SPI TXD Buffer Empty
	SPI_SendData8(M8266WIFI_INTERFACE_SPI, TxdByte); 			//Write the byte to the TXD buffer and then shift out via MOSI
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_RXNE) == RESET){} 	//wait the SPI RXD buffer has received the data from MISO
	return SPI_ReceiveData8(M8266WIFI_INTERFACE_SPI);  		//return the data received from MISO
#elif defined(MCU_IS_STM32F4XX_HAL) || defined(MCU_IS_STM32F7XX) || defined(MCU_IS_STM32L1XX) || defined(MCU_IS_STM32L4XX) || defined(MCU_IS_STM32H7XX)
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&hspi,&TxdByte,&Rxdata,1, 1000);       
 	return Rxdata; 
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif	
}
