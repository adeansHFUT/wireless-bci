#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "brd_cfg.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "main.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "M8266WIFIDrv.h"


void M8266WIFI_Test(void);
uint32_t x = 0x00000038U;
int main(void)
{
	uint8_t success=0;
  HAL_Init();
	SystemClock_Config(8,336,2,7);    //void SystemClock_Config(int pllm,int plln,int pllp,int pllq)
	delay_Init();
	M8266HostIf_Init();	    //WIFISPI初始化
	SPI2_Init(x);     //intan口初始化同时完成intan采集数据的发送  这里需要更改波特率！
	success = M8266WIFI_Module_Init_Via_SPI(); //成功就返回1，不成功就返回0的，只是表示初始化是否成功
  if(success)
	{}	
	else // If M8266WIFI module initialisation failed, two led constantly flash in 2Hz (Chinese: 以2Hz频率持续同时闪烁单片机主板上的2个LED灯，表示M8266WIFI模组初始化失败了)
	{
		  while(1)
			{}
	}

	M8266WIFI_Test();
}