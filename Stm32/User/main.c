/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPI FLASH基本读写例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_spi.h" 
#include "sdio_sdcard.h"
#include "malloc.h"       
#include "ff.h"  
#include "exfuns.h"  
#include "fattester.h"
#include "rtc.h"
#include "delay.h"


//#define RX_LEN 		    	(uint8_t)35
//#define TX_LEN          (uint8_t)35
//typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

///* 获取缓冲区的长度 */
////#define TxBufferSize1   (countof(TxBuffer1) - 1)
////#define RxBufferSize1   (countof(TxBuffer1) - 1)
////#define countof(a)      (sizeof(a) / sizeof(*(a)))
////#define  BufferSize (countof(Tx_Buffer)-1)

//uint16_t SPI_TX_BUFFER[TX_LEN] = {CONVERT0,CONVERT1};
//   
///* 发送缓冲区初始化 */
////uint16_t Tx_Buffer[] =1,2;
////uint16_t Rx_Buffer[BufferSize];

//读取的ID存储位置
//__IO uint32_t DeviceID = 0;
//__IO uint32_t FlashID = 0;
#define  INTAN_ID = 1    //
         int i = 0;
	       u16 count = 0;
	       u16 Temp = 0;
				 u16 x = 0x0008;
				 u8 sign1= 0;
				 u8 sign2= 0;
				 u8 sign3= 0;
				 u8 sign4= 0;
				 u8 sign5= 0;
				 u8 sign6= 0;
				 u8 sign7= 0;
				 u8 sign8= 0;
         u8 sign9= 0;
				 u8 sign10= 0;
         u8 sign11= 0;
				 u8 sign12= 0;
				 u8 sign13= 0;
				 u8 sign14= 0;
				 u8 sign15= 0;
				 u8 sign16= 0;
				 u8 sign17= 0;
				 u8 sign18= 0;
				 u8 sign19= 0;
         u8 sign20= 0;
				 u8 sign21= 0;
				 u8 sign22= 0;
				 u8 sign23= 0;
				 u8 sign24= 0;
				 u8 sign25= 0;
				 u8 sign26= 0;
				 u8 sign27= 0;
				 u8 sign28= 0;
         u8 sign29= 0;
				 u8 sign30= 0;
				 u8 sign31= 0;
				 u8 sign32= 0;
				 u8 sign33= 0;
				 u8 sign34= 0;
				 u8 sign35= 0;
				 u8 sign36= 0;
				 u8 sign37= 0;
         u8 sign38= 0;
				 u8 sign39= 0;
				 u8 sign40= 0;
				 u8 sign41= 0;
         u8 sign42= 0;
				 u8 sign43= 0;
				 u8 sign44= 0;
				 u8 sign45= 0;

				extern u16 SPI_RX_BUFFER[1];	
        extern u16 SPI_TX_BUFFER[35];
				extern u16 SPI_TX_intan[5];
u8 flag = 0;
extern int cnt_count;
volatile long int block_num=9000;
  FIL fil;
  FRESULT fr;

FRESULT open_append_me (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
//        fr = f_lseek(fp, f_size(fp));
			  fr = f_lseek(fp, fp->fsize);
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}


// 函数原型声明
void Delay(__IO uint32_t nCount);


/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{ 
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	uint16_t data[5] = {0xfe12,0xfd13,0xfd14,0x15,0xfd16};
	int cnt=0;



	long int tmp_num = 1;

	uint16_t tmpbuf1[4096];
//	u8 *tmpbuf1;
	u8 *buf;
	uint16_t tmpbuf2[32];
	UINT bw;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
  uart_init(57600);
//	SPI1_Init();
		SPI1_Init(x); //SPI_BaudRatePrescaler_2         ((uint16_t)0x0000) 
								//SPI_BaudRatePrescaler_4         ((uint16_t)0x0008)10进制 8  总采样率20K
								//SPI_BaudRatePrescaler_8         ((uint16_t)0x0010)10进制 16 总采样率10K
								//SPI_BaudRatePrescaler_16        ((uint16_t)0x0018)10进制 24 总采样率5K
								//SPI_BaudRatePrescaler_32        ((uint16_t)0x0020)10进制 32 总采样率2.5K
								//SPI_BaudRatePrescaler_64        ((uint16_t)0x0028)10进制 40
								//SPI_BaudRatePrescaler_128       ((uint16_t)0x0030)10进制 48
								//SPI_BaudRatePrescaler_256       ((uint16_t)0x0038)10进制 56
	my_mem_init(SRAMIN);
	while(SD_Init()){};
 	exfuns_init();							//?afatfs?à1?±?á?éê???ú′?				 
  f_mount(fs[0],"0:",1);

	
		/* 初始化芯片配置寄存器 */
	
	   SPI_CS_LOW(); Delay( 50 );
	 
	   SPI_SendHalfWord(0x80fe);
	
     Delay( 50 );SPI_CS_HIGH();
		
	   Delay( 84000 );
	 
	 /* 初始化芯片配置寄存器0：ADC配置和放大器快速建立 */
	 	SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x80de);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
	 	/* 初始化芯片配置寄存器1：传感器采样和ADC缓冲器偏置电流设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8120);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		/* 初始化芯片配置寄存器2：传感器采样和ADC缓冲器偏置电流设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8228);

		Delay( 50 );SPI_CS_HIGH();
	
		Delay( 500 );
	 	/* 初始化芯片配置寄存器3：温度传感器和辅助数字输出设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8301);
	
		Delay( 50 );SPI_CS_HIGH();
		
    Delay( 500 );
	
	 	/* 初始化芯片配置寄存器4：ADC输出格式和DSP失调消除设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8480);

		Delay( 50 );SPI_CS_HIGH();
		
    Delay( 500 );
		
	 	/* 初始化芯片配置寄存器5：阻抗检测控制（未开启）设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8500);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器6：寄存器5的DAC设置（置零）设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8600);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
	 	/* 初始化芯片配置寄存器7：同上置零设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8700);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器8：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x881e);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器9：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器10：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8a2b);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器11：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8b86);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );

	 	/* 初始化芯片配置寄存器12：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8c10);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );	
						
	 	/* 初始化芯片配置寄存器13：放大器带宽设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8dfc);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器14：放大器启动设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8eff);
    
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
	 	/* 初始化芯片配置寄存器15：放大器启动设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x8fff);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
	 	/* 初始化芯片配置寄存器16：放大器启动设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x90ff);
	
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
	 	/* 初始化芯片配置寄存器17：放大器启动设置 */
	 
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x91ff);
	
		Delay( 50 );SPI_CS_HIGH();
		
		/* 寄存器初始化后100us进行ADC校准,校准开始后需要额外9命令时钟 */
    Delay( 20000 );
		
	  SPI_CS_LOW(); Delay( 50 );
	 
	  SPI_SendHalfWord(0x5500);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff0);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );

		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff1);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff2);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff3);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff4);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff5);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff6);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff7);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff8);
		
		Delay( 50 );SPI_CS_HIGH();
		
		Delay( 500 );
		
		SPI_CS_LOW(); Delay( 50 );
		
		SPI_SendHalfWord(0xfff9);
		
		Delay( 50 );SPI_CS_HIGH();
		
    Delay( 5000 );
		

	
	while(1)
	{
		 if(sign1)
		{ 
			x=0x0008;		//84MHz-4分频
	    SPI1_Init(x);
			uart_init(921600);
			Usart_SendHalfWord(USART1,20000);
		}
		 if(sign2)
		{ 
			x=0x0010;		//84MHz-8分频
	    SPI1_Init(x);
			uart_init(460800);
			Usart_SendHalfWord(USART1,10000);
		}
		 if(sign3)
		{ 
			x=0x0028;		//84MHz-64分频
	    SPI1_Init(x);
			uart_init(256000);
			Usart_SendHalfWord(USART1,1250);
		}
		 if(sign4)
		{ 
			x=0x0038;		//84MHz-256分频
	    SPI1_Init(x);
			uart_init(115200);
			Usart_SendHalfWord(USART1,300);
		}
		
		 if(sign12)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[0]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}
		
		 if(sign13)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[1]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign14)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[2]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign15)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[3]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign16)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[4]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign17)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[5]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign18)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[6]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign19)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[7]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign20)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[8]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign21)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[9]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign22)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[10]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign23)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[11]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign24)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[12]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign25)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[13]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign26)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[14]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign27)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[15]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign28)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[16]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign29)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[17]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign30)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[18]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign31)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[19]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign32)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[20]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign33)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[21]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign34)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[22]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign35)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[23]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign36)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[24]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign37)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[25]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign38)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[26]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign39)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[27]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign40)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[28]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign41)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[29]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign42)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[30]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		 if(sign43)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[31]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		if(sign44)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[33]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}
		
		if(sign45)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[34]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));						
		}

		
		 if(sign6)
		{
			for (i=0;i<32;i++)
		{
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
		}
			if(i==32)
		{ 
			Usart_SendHalfWord(USART1,0xFFFF);
			i=0;
		}		
		}
		
			if(sign7)
		{
			for (i=0;i<35;i++)
		{
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
		}
	  }

//			if(sign11)
//		{
//			for (i=0;i<35;i++)
//		{
//			SPI_CS_LOW();
//			
//			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

//			SPI_CS_HIGH();
//			
//			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
//			
//			i = i+1;
//		}
//	  }

//			if(sign10)
//		{
//			for (i=1;i<35;i++)
//		{
//			SPI_CS_LOW();
//			
//			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

//			SPI_CS_HIGH();
//			
//			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
//			
//			i = i+1;
//		}
//	  }
		
//		 if(sign8)
//		{
//			
//			SPI_CS_LOW();
//			
//			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

//			SPI_CS_HIGH();
//			
//			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
//			
//			SPI_CS_LOW();
//			
//			SPI_SendHalfWord(SPI_TX_BUFFER[i+1]);

//			SPI_CS_HIGH();
//			
//			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));	
//			
//			SPI_CS_LOW();
//			
//			SPI_SendHalfWord(SPI_TX_BUFFER[i+2]);

//			SPI_CS_HIGH();
//			
//			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));				
//		}
		
    if(sign9)
		{
	    for (i=0;i<35;i++)
		{
					
			SPI_CS_LOW();
 
			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

			SPI_CS_HIGH();
			
			tmpbuf1[cnt] = SPI_I2S_ReceiveData(SPI1);

			cnt++;
			if(cnt == 4096)
			{
				
				SD_WriteDisk((u8*)tmpbuf1,block_num,16);
				block_num = block_num+16;			
				cnt=0;
				
			}
		}
	}
	if(sign10)
	{
		int j = 0;
		for(j = 0; j < 5; j++)
		{
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_intan[j]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART1,SPI_I2S_ReceiveData(SPI1));
		}
	}
			
}
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
