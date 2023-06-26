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
	M8266HostIf_Init();	    //WIFISPI��ʼ��
	SPI2_Init(x);     //intan�ڳ�ʼ��ͬʱ���intan�ɼ����ݵķ���  ������Ҫ���Ĳ����ʣ�
	success = M8266WIFI_Module_Init_Via_SPI(); //�ɹ��ͷ���1�����ɹ��ͷ���0�ģ�ֻ�Ǳ�ʾ��ʼ���Ƿ�ɹ�
  if(success)
	{}	
	else // If M8266WIFI module initialisation failed, two led constantly flash in 2Hz (Chinese: ��2HzƵ�ʳ���ͬʱ��˸��Ƭ�������ϵ�2��LED�ƣ���ʾM8266WIFIģ���ʼ��ʧ����)
	{
		  while(1)
			{}
	}

	M8266WIFI_Test();
}