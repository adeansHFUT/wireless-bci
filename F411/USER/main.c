#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "malloc.h" 
#include "sdio_sdcard.h"  
#include "spi.h"
#include "stm32f4xx.h"
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
				 u8 sign11 = 0;
				 u16 x = 0x0010;//SPI_BaudRatePrescaler 8 
				 u16 delay_time = 100;
				 u8 first_acquire_circle = 0;
				 u8 first_test_sd = 0;
				 
		     extern u16 SPI_RX_BUFFER[1];	
         extern u16 SPI_TX_BUFFER[35];
				 extern u16 SPI_TX_BUFFER_2[32];
				 extern u16 SPI_TX_intan[5]; //AScii intan
					uint8_t temp_random[512] = {0}; //random code write to sd for indicate where store begin and finish
					uint8_t tmpbuf1[66*256];  // for sd card store
         volatile u32 block_num=32768;  // must >= 32768, winhex can export it
				 extern SD_Error TransferError;
				 extern u8 TransferEnd;



// Global variable to store the seed value
volatile uint32_t seed = 0;

void TIM2_Init(void) {
    // Enable the clock for TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Set the timer prescaler and period
    // For example, to achieve a 1 ms tick with a 16 MHz timer clock:
    TIM2->PSC = 15999; // Prescaler (16 MHz / (15999 + 1) = 1000 Hz)
    TIM2->ARR = 999;   // Auto-reload value (1 ms - 1)

    // Enable the update event for TIM2 (this will update the prescaler and period)
    TIM2->EGR |= TIM_EGR_UG;

    // Start the timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

// Function to initialize the seed using a timer count or any other method
void InitSeed() {
    // Replace "TIMx" with the timer instance you want to use for generating the seed
    seed = TIM2->CNT;
}

// Function to generate a pseudo-random number using LCG algorithm
uint32_t Get_Pseudo_Random_Number() {
    // Constants for LCG algorithm
    const uint32_t LCG_MULTIPLIER = 1103515245;
    const uint32_t LCG_INCREMENT = 12345;
    const uint32_t LCG_MODULUS = 0x80000000;

    // Update seed with LCG algorithm
    seed = (LCG_MULTIPLIER * seed + LCG_INCREMENT) % LCG_MODULUS;

    return seed;
}

int main(void)
{ 
	uint16_t sdtemp_cnt=0;		 
	int count_bluetooth = 0;
	u8 sd_status = 0;
	u8 t=0;	
	u8 *buf;
	uint16_t tmpbuf2[32]; // for bluetooth send 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(96);  //初始化延时函数
	uart_init(1000000);		//初始化串口波特率为115200
	SPI1_Init(0x0000); 
	TIM2_Init();
	delay_ms(1000); // delay for ble to connect automatically
	
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
/********************************32 channel send to pc with verification****************************************/
		 if(sign6)
		{
			
			u8 j;
			
			if(first_acquire_circle)  // discard the first two words 
			{
				SPI_CS_LOW();
				SPI_SendHalfWord(CONVERT0);
				SPI_CS_HIGH();
				SPI_CS_LOW();
				SPI_SendHalfWord(CONVERT1);
				SPI_CS_HIGH();
				first_acquire_circle = 0;
			}
						
			for (i=0;i<32;i++)  // channels sended are from 0 to 31
			{
				SPI_CS_LOW();
					
				SPI_SendHalfWord(SPI_TX_BUFFER_2[i]);

				SPI_CS_HIGH();
					
				//Usart_SendHalfWord(USART6, SPI_I2S_ReceiveData(SPI1));
					
				tmpbuf2[i] = SPI_I2S_ReceiveData(SPI1);
			}

			for(j = 0; j < 32; j++ )
			{
				Usart_SendHalfWord(USART6, tmpbuf2[j]);
				//delay_us(2);
			}
				
			Usart_SendHalfWord(USART6,0x1234);
			//Usart_SendHalfWord(USART6,0x5678);
			//Usart_SendHalfWord(USART6,0xFFFF);	
			//delay_us(50);
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
/********************************finish store sd card****************************************/			
		 if(sign8)
		{
			uint8_t i2;
			temp_random[4] = 0x22;
			temp_random[5] = 0x22;
			temp_random[6] = (block_num&0XFF000000)>>24;
			temp_random[7] = (block_num&0X00FF0000)>>16;
			temp_random[8] = (block_num&0X0000FF00)>>8;
			temp_random[9] = block_num&0X000000FF;
			sd_status += SD_WriteDisk((u8*)temp_random,block_num,1);
			block_num++;
			for(i2=0;i2<10;i2++)
			{
				USART_SendData(USART6,temp_random[i2]);	
				while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
			}
			sign8 = 0;
			sign9 = 0;
			
		}
/********************************store sd card****************************************/							
		if(sign9)
		{
			uint16_t spi_16;
		/****************if first call***************************/	
			if(first_test_sd)
			{
			  uint32_t randomValue;
				uint8_t temp_8;
				uint8_t i1;
				while(SD_Init())//检测不到SD卡
				{
					//Usart_SendHalfWord(USART6,0x1122);
					delay_ms(500);
					NVIC_SystemReset();  //reset 
				}
		/****************discard first two command****************************/
				SPI_CS_LOW();
				SPI_SendHalfWord(CONVERT0);
				SPI_CS_HIGH();
				SPI_CS_LOW();
				SPI_SendHalfWord(CONVERT1);
				SPI_CS_HIGH();
		/****************generate random word****************************/		
				// Generate and use a random number
				InitSeed();
				randomValue = Get_Pseudo_Random_Number();
				// Use the randomValue as needed
					/* 取出高八位 */
				temp_random[0] = (randomValue&0XFF000000)>>24;
				temp_random[1] = (randomValue&0X00FF0000)>>16;
				temp_random[2] = (randomValue&0X0000FF00)>>8;
				temp_random[3] = randomValue&0X000000FF;
				temp_random[4] = 0x11;
				temp_random[5] = 0x11;
				temp_random[6] = (block_num&0XFF000000)>>24;
				temp_random[7] = (block_num&0X00FF0000)>>16;
				temp_random[8] = (block_num&0X0000FF00)>>8;
				temp_random[9] = block_num&0X000000FF;
		/****************random word write sd****************************/
				sd_status += SD_WriteDisk((u8*)temp_random,block_num,1);
				block_num++;
				
		/****************random word send to PC****************************/				
				for(i1=0;i1<10;i1++)
				{
					USART_SendData(USART6,temp_random[i1]);	
					while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
				}
				first_test_sd = 0;
				sdtemp_cnt = 0;
			}
/****************************cycle call*********************/		
	    for (i=0;i<32;i++)
			{
						
				SPI_CS_LOW();
	 
				SPI_SendHalfWord(SPI_TX_BUFFER_2[i]);

				SPI_CS_HIGH();
				
				spi_16 = SPI_I2S_ReceiveData(SPI1);
				tmpbuf1[sdtemp_cnt] = (spi_16&0XFF00)>>8;
				tmpbuf1[sdtemp_cnt+1] = spi_16&0X00FF;
				sdtemp_cnt+=2;
			}
			tmpbuf1[sdtemp_cnt] = 0x12; // indicate a record cycle
			tmpbuf1[sdtemp_cnt+1] = 0x34; // indicate a record cycle
			sdtemp_cnt+=2;
			if(sdtemp_cnt == 66*256)
			{
							u32 timeout = SDIO_DATATIMEOUT;
							SD_Error errorstatus = SD_OK;
							u8 cardstate = 0;
			/*******************************等待上一次DMA传输完成 ***************************************/
							while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待传输完成 
							if(timeout==0)	 								//超时
							{									  
									SD_Init();	 					//重新初始化SD卡,可以解决写入死机的问题
								return SD_DATA_TIMEOUT;			//超时	 
							}
							timeout=SDIO_DATATIMEOUT;
							while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
							if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
							if(TransferError!=SD_OK)return TransferError;	
			/*******************************清除所有sd标记 ***************************************/		
							SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
							errorstatus=IsCardProgramming(&cardstate);
							while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
							{
								errorstatus=IsCardProgramming(&cardstate);
							}
						
			/*******************************start sd DMA with no wait***************************************/			
							SD_WriteDisk_nowait((u8*)tmpbuf1,block_num,33);
							block_num = block_num+33;
							sdtemp_cnt=0;					
			}
		}
		
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
		if(sign11) 
		{
			uint16_t m = 0;
			for(m = 0; m < 65535; m++)
			{
				//tmpbuf1[sdtemp_cnt] = m;
				Usart_SendHalfWord(USART6, m);
				//sdtemp_cnt++;
				//if(sdtemp_cnt == 4096)
				//{
					//SD_WriteDisk((u8*)tmpbuf1,block_num,16);
					//block_num = block_num+16;			
					//sdtemp_cnt=0;
				//}
			}
				
		}
		
}

}



