#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "brd_cfg.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "main.h"

void M8266WIFI_Test(void);
int main(void)
{
	u8 success=0;
	
  HAL_Init();

	// Modify below according to the external cystal to the STM32F4 MCU, you need modify two points! 
  //( Chinese: ������������ϵ�Ƭ��ʹ�õ��ⲿ����Ƶ�ʣ������޸�����Ĳ���
  // ע�⣬Ҫ���޸�2���ط�������
	//���޸�1��
  // system Clocks Initialise according to extern crystal           (Chinese: �����ⲿ�������ϵͳʱ�����ã�
	// if external crystal is 8MHz, e.g. on most platforms of STM32F407   (Chinese: ���������ⲿ������8MHz��ʹ�����������ϵͳʱ�����ã������407������ʹ��8MHz)
	SystemClock_Config(8,336,2,7);    //void SystemClock_Config(int pllm,int plln,int pllp,int pllq);
	// if external crystal is 25MHz, e.g on most platforms fo STM32F429   (Chinese: ���������ⲿ������25MHz��ʹ�����������ϵͳʱ�����ã������429������ʹ��25MHz)
	//SystemClock_Config(25,336,2,7);   //void SystemClock_Config(int pllm,int plln,int pllp,int pllq);
  //���޸�2��
	// Meanwhile, Please remember to update the HSE_VALUE in user/stm32f4xx_hal_conf.h  
	//(Chinese:  ͬʱ���ǵ���USERĿ¼�µ�stm32f4xx_hal_conf.h�У����궨��HSE_VALUE����ֵ�ĳ���Ӧ��8M��25M)  
	
	delay_Init();
	
#ifdef USE_LED_AND_KEY_FOR_TEST  //If Leds and keys are present in the MCU board, you could use them to coordinate the demo. Not Mandatory. (Chinese: �����Ƭ����������LED�ƺͰ�������������������ϲ��Ե���ʾ�������Ǳ��룩
  LED_Init();				// Initialise LEDs
  KEY_Init();				// Initialise KEYs
#endif
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//1. Call M8266HostIf_Init() to initialize the MCU Host Interface (Chinese: ����M8266HostIf_Init()����ʼ����Ƭ�������ӿ� )
	//   - including SPI and nREST/nCS GPIO pins                      (Chinese: - ����SPI��nRESET/nCS GPIO�ܽ� )
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	M8266HostIf_Init();
	
#ifdef USE_LED_AND_KEY_FOR_TEST	
  u8 i;	
  //Flash 5 times both of the two LEDs simutaneously indicating that next will initialisation the M8266WIFI module (Chinese: ͬʱ��˸��Ƭ�������ϵ�2��LED�ƣ���ʾ��һ��׼����ʼ��M8266WIFIģ��)
	for(i=0; i<5; i++)
  {
		LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(50);
		LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(50);
	}
#endif
	
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//2. Call M8266WIFI_Module_Init_Via_SPI() to initialize the wifi module via SPI Interface  (Chinese: ����M8266WIFI_Module_Init_Via_SPI()ͨ��SPI�ӿ�����ʼ��wifiģ�� )
  //   - Including: Module reset, module select, module connecting wifi if required, and etc (Chinese: - ������λwifiģ�飬 ִ��ģ��ѡ�С���Ҫʱ��ģ������·�����ȵ�ȵȲ��� )	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	success = M8266WIFI_Module_Init_Via_SPI();
  if(success)
	{
#ifdef USE_LED_AND_KEY_FOR_TEST			
	    for(i=0; i<3; i++)  // Flash 3 times the Two LEDs alternatively in the Main Board indicating M8266WIFI initialised successfully (Chinese: ������˸��Ƭ�������ϵ�2��LED��3�Σ���ʾM8266WIFIģ���ʼ���ɹ�)
	    {
	       LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			}
		  LED_set(0, 1); LED_set(1, 1);
#endif
	}	
	else // If M8266WIFI module initialisation failed, two led constantly flash in 2Hz (Chinese: ��2HzƵ�ʳ���ͬʱ��˸��Ƭ�������ϵ�2��LED�ƣ���ʾM8266WIFIģ���ʼ��ʧ����)
	{
		  while(1)
			{
#ifdef USE_LED_AND_KEY_FOR_TEST					
	       LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(250);
		     LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(250);
#endif
			}
	}

  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//3. Call M8266WIFI_Test() to setup socket connection and tansmit/receive data test in a high-speed manner (Chinese: ����M8266WIFI_Test()�������׽������Ӻ͸���ͨ���շ����� )
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	M8266WIFI_Test(); //

	
	while(1)
	{
#ifdef USE_LED_AND_KEY_FOR_TEST					
	       LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(1000);
		     LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(1000);
#endif
	}
} //end of main 
