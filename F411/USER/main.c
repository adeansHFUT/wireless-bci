#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "malloc.h" 
#include "sdio_sdcard.h"  
#include "spi.h"
//ALIENTEK 探索者STM32F407开发板 实验38
//SD卡 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 
         int i = 0 ;
				 u8 sign1= 0;
				 u8 sign2= 0;
				 u8 sign3= 0;
				 u8 sign4= 0;
				 u8 sign5= 0;
				 u8 sign6= 0;
				 u8 sign7= 0;
				 u8 sign8= 0;
         u8 sign9= 0;
				 u8 sign10 = 0;
				 u16 x = 0x0010;//SPI_BaudRatePrescaler 8 

		     extern u16 SPI_RX_BUFFER[1];	
         extern u16 SPI_TX_BUFFER[35];
				 extern u16 SPI_TX_intan[5]; //AScii intan

         volatile long int block_num=100;



int main(void)
{        
			 
	int cnt=0;
	u8 t=0;	
	u8 *buf;
  uint16_t tmpbuf1[4096];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(96);  //初始化延时函数
	uart_init(230400);		//初始化串口波特率为115200
	SPI1_Init(x);
	delay_ms(3000); // delay for ble to connect automatically
	Usart_SendHalfWord(USART6,0x1234); // log
	
	//my_mem_init(SRAMIN);		//初始化内部内存池 
	//my_mem_init(SRAMCCM);		//初始化CCM内存池
		   
 	while(SD_Init())//检测不到SD卡
	{
		Usart_SendHalfWord(USART6,0x1122);
		delay_ms(500);
	}
		
	delay_ms(1000);
		
 

	while(1)
	{
     if(sign1)
		{ 
			x=0x0000;		
	    SPI1_Init(x);
			Usart_SendHalfWord(USART6,20000);
		}
		 if(sign2)
		{ 
			x=0x0008;		
	    SPI1_Init(x);
			Usart_SendHalfWord(USART6,10000);
		}
		 if(sign3)
		{ 
			x=0x0020;		
	    SPI1_Init(x);
			Usart_SendHalfWord(USART6,1250);
		}
		 if(sign4)
		{ 
			x=0x0038;		
	    SPI1_Init(x);
			Usart_SendHalfWord(USART6,150);
		}
		
		 if(sign5)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));						
		}
		 if(sign6)
		{
			for (i=0;i<32;i++)
		{
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[i]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));
		}
			if(i==32)
		{ 
			Usart_SendHalfWord(USART6,0xFFFF);
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
			
			Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));
		}
	  }
			
		 if(sign8)
		{
			
			SPI_CS_LOW();
			
			SPI_SendHalfWord(SPI_TX_BUFFER[34]);

			SPI_CS_HIGH();
			
			Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));						
		}
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
		else
			cnt = 0; //avoid store last cycle data after another call sign9
		
	if(sign10)  // add intan character send function
		{
				int j = 0;
				for(j = 0; j < 5; j++)
				{
					SPI_CS_LOW();
					
					SPI_SendHalfWord(SPI_TX_intan[j]);

					SPI_CS_HIGH();
					
					Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));
				}
		}		
}

}



