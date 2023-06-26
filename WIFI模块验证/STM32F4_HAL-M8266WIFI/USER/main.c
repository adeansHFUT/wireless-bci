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
  //( Chinese: 根据你的主板上单片机使用的外部晶体频率，调整修改下面的部分
  // 注意，要【修改2个地方】！）
	//【修改1】
  // system Clocks Initialise according to extern crystal           (Chinese: 根据外部晶体进行系统时钟配置）
	// if external crystal is 8MHz, e.g. on most platforms of STM32F407   (Chinese: 如果主板的外部晶体是8MHz，使用这条语句做系统时钟配置，大多数407主板上使用8MHz)
	SystemClock_Config(8,336,2,7);    //void SystemClock_Config(int pllm,int plln,int pllp,int pllq);
	// if external crystal is 25MHz, e.g on most platforms fo STM32F429   (Chinese: 如果主板的外部晶体是25MHz，使用这条语句做系统时钟配置，大多数429主板上使用25MHz)
	//SystemClock_Config(25,336,2,7);   //void SystemClock_Config(int pllm,int plln,int pllp,int pllq);
  //【修改2】
	// Meanwhile, Please remember to update the HSE_VALUE in user/stm32f4xx_hal_conf.h  
	//(Chinese:  同时，记得在USER目录下的stm32f4xx_hal_conf.h中，将宏定义HSE_VALUE的数值改成相应的8M或25M)  
	
	delay_Init();
	
#ifdef USE_LED_AND_KEY_FOR_TEST  //If Leds and keys are present in the MCU board, you could use them to coordinate the demo. Not Mandatory. (Chinese: 如果单片机主板上有LED灯和按键，可以用它们来配合测试的提示操作。非必须）
  LED_Init();				// Initialise LEDs
  KEY_Init();				// Initialise KEYs
#endif
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//1. Call M8266HostIf_Init() to initialize the MCU Host Interface (Chinese: 调用M8266HostIf_Init()来初始化单片机主机接口 )
	//   - including SPI and nREST/nCS GPIO pins                      (Chinese: - 包括SPI和nRESET/nCS GPIO管脚 )
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	M8266HostIf_Init();
	
#ifdef USE_LED_AND_KEY_FOR_TEST	
  u8 i;	
  //Flash 5 times both of the two LEDs simutaneously indicating that next will initialisation the M8266WIFI module (Chinese: 同时闪烁单片机主板上的2个LED灯，表示下一步准备初始化M8266WIFI模组)
	for(i=0; i<5; i++)
  {
		LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(50);
		LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(50);
	}
#endif
	
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//2. Call M8266WIFI_Module_Init_Via_SPI() to initialize the wifi module via SPI Interface  (Chinese: 调用M8266WIFI_Module_Init_Via_SPI()通过SPI接口来初始化wifi模组 )
  //   - Including: Module reset, module select, module connecting wifi if required, and etc (Chinese: - 包括复位wifi模组， 执行模组选中、必要时让模块连接路由器热点等等操作 )	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	success = M8266WIFI_Module_Init_Via_SPI();
  if(success)
	{
#ifdef USE_LED_AND_KEY_FOR_TEST			
	    for(i=0; i<3; i++)  // Flash 3 times the Two LEDs alternatively in the Main Board indicating M8266WIFI initialised successfully (Chinese: 交替闪烁单片机主板上的2个LED灯3次，表示M8266WIFI模组初始化成功)
	    {
	       LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			}
		  LED_set(0, 1); LED_set(1, 1);
#endif
	}	
	else // If M8266WIFI module initialisation failed, two led constantly flash in 2Hz (Chinese: 以2Hz频率持续同时闪烁单片机主板上的2个LED灯，表示M8266WIFI模组初始化失败了)
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
	//3. Call M8266WIFI_Test() to setup socket connection and tansmit/receive data test in a high-speed manner (Chinese: 调用M8266WIFI_Test()来创建套接字链接和高速通信收发数据 )
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
