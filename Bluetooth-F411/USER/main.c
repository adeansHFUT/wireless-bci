#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "malloc.h" 
#include "sdio_sdcard.h"  
#include "spi.h"
#include "stm32f4xx.h"

/************
README:
	record 1 cycle use about 40us
	store 16 sector use about 4ms
	
	SD_CYCLE_SOTRE = floor(SD_MEM_U16/66)
	SD_MEM_U16 = SD_WRITE_SECETOR_NUM*512
****************/

#define SD_CYCLE_SOTRE 426   
#define SD_MEM_U16 28160
#define SD_WRITE_SECETOR_NUM 55  

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
				 u8 sd_dma_num;
				 
		     extern u16 SPI_RX_BUFFER[1];	
         extern u16 SPI_TX_BUFFER[35];
				 extern u16 SPI_TX_BUFFER_2[32];
				 extern u16 SPI_TX_intan[5]; //AScii intan
					uint8_t temp_random[512] = {0}; //random code write to sd for indicate where store begin and finish
					uint16_t tmpbuf_sd[2][SD_MEM_U16] = {0};  // for sd card store, dual DMA buffer
         u32 block_num=32768;  // must >= 32768, winhex can export it
				 extern SD_Error TransferError;
				 extern u8 TransferEnd;
				extern SDIO_InitTypeDef SDIO_InitStructure;
				extern SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
				extern SDIO_DataInitTypeDef SDIO_DataInitStructure;   

uint32_t SD_CYCLE_TIME = 430;  // about 18.5K Sample rate
// SD_CYCLE_TIME 470  // about 21.27K Sample rate
// SD_CYCLE_TIME 500  // about 20K Sample rate
extern u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);
extern SD_Error CmdResp1Error(u8 cmd);
// Global variable to store the seed value
uint32_t sdtemp_cnt=0;	
uint32_t seed = 0;
uint32_t time1,time2,time3,time4=0;
uint32_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20=0;
uint32_t real_sd_cycle = 0;
extern u8 CardType;		//SD卡类型（默认为1.x卡）
extern u32 CSD_Tab[4],CID_Tab[4],RCA;					//SD卡CSD,CID以及相对地址(RCA)数据
extern u8 DeviceMode;		   				//工作模式,注意,工作模式必须通过SD_SetDeviceMode,后才算数.这里只是定义一个默认的模式(SD_DMA_MODE)
extern u8 StopCondition; 								//是否发送停止传输标志位,DMA多块读写的时候用到  
extern SD_CardInfo SDCardInfo;									//SD卡信息

void TIM2_Init(void) {
    // Enable the clock for TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Set the timer prescaler and period
    // For example, to achieve a 1 ms tick with a 16 MHz timer clock:
    TIM2->PSC = 5; // Prescaler (50 MHz / 5 = 10MHz)
    TIM2->ARR = 9999999;   // Auto-reload value (1s) precion 0.1us

    // Enable the update event for TIM2 (this will update the prescaler and period)
    TIM2->EGR |= TIM_EGR_UG;

    // Start the timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

// Function to initialize the seed using a timer count or any other method
uint32_t InitSeed() {
    // Replace "TIMx" with the timer instance you want to use for generating the seed
    seed = TIM2->CNT;
	  return seed;
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

void wait_until_xus(void)
{
	while(1){	
		if(TIM2->CNT - time1 > 0){
			if(TIM2->CNT -time1 >= SD_CYCLE_TIME){
				real_sd_cycle = TIM2->CNT - time1;
				break;
			}
		}else{
			if(TIM2->CNT - time1 + 10000000 >= SD_CYCLE_TIME){
				real_sd_cycle = TIM2->CNT - time1 + 10000000;
				break;
			}
		}			
	}
}
	

int main(void)
{ 
	/*********SD PARAMETER*************/
	u16 blksize = 512;
	u32 nblks = SD_WRITE_SECETOR_NUM;
	u8 sd_status = 0;
	u8 t=0;	
	u8 *buf;
	uint32_t sd_cycle = 0;
	u32 timeout = SDIO_DATATIMEOUT;
	SD_Error errorstatus = SD_OK;
	u8 cardstate = 0;
	u8  power = 0;
	long long addr = 0;
 /*********BLUETOOTH PARAMETER*************/
	int count_bluetooth = 0;
	uint16_t tmpbuf2[32]; // for bluetooth send 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(100);  //初始化延时函数
	uart_init(1000000);		//初始化串口波特率
	SPI1_Init(0x0000); 
	TIM2_Init();
	delay_ms(1000); // delay for ble to connect automatically

	while(1)
	{

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
					
				tmpbuf2[i] = SPI1->DR;
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
			delay_ms(500);  // wait until last sd store finish
			//SD_PowerOFF();
			while(SD_Init())//检测不到SD卡
			{
					Usart_SendHalfWord(USART6,0x9999);
					Usart_SendHalfWord(USART6,0x9999);
					delay_ms(250);
					//NVIC_SystemReset();  //reset 
			}	
			sd_status += SD_WriteDisk((u8*)temp_random,block_num,1);
			delay_ms(100);
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
		/****************if first call***************************/	
			if(first_test_sd)
			{
			  uint32_t randomValue;
				uint8_t temp_8;
				uint8_t i1;
				while(SD_Init())//检测不到SD卡
				{
					Usart_SendHalfWord(USART6,0x9999);
					Usart_SendHalfWord(USART6,0x9999);
					delay_ms(250);
					//NVIC_SystemReset();  //reset 
				}
		/****************discard first two command****************************/
				SPI_CS_LOW();
				SPI_SendHalfWord(CONVERT0);
				SPI_CS_HIGH();
				delay_us(1);
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
				sd_dma_num = 0;
				sd_cycle = 0;
			}
			time1 = TIM2->CNT;
/****************************cycle call 32 channel*********************/	
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT2);	

			SPI_CS_HIGH();
			 					 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT3);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT4);	

			SPI_CS_HIGH();
			 					 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT5);	

			SPI_CS_HIGH();
			 		 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT6);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT7);	

			SPI_CS_HIGH();
			 				 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT8);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT9);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT10);	

			SPI_CS_HIGH();
			 					 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT11);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT12);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT13);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT14);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT15);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT16);	

			SPI_CS_HIGH();			 
						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT17);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT18);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT19);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT20);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT21);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT22);	

			SPI_CS_HIGH();
			 					 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT23);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT24);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT25);	

			SPI_CS_HIGH();
			 						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT26);	

			SPI_CS_HIGH();
						 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT27);	

			SPI_CS_HIGH();		
			 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT28);	

			SPI_CS_HIGH();
			 		 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT29);	

			SPI_CS_HIGH();	
			 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT30);	

			SPI_CS_HIGH();			 			
			 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT31);	

			SPI_CS_HIGH();			 			
			 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT0);	

			SPI_CS_HIGH();
			 
			sdtemp_cnt++;
			
			SPI_CS_LOW();
 
			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = SPI_SendHalfWord(CONVERT1);	

			SPI_CS_HIGH();

			sdtemp_cnt++;

			tmpbuf_sd[sd_dma_num][sdtemp_cnt] = 0x1234; // indicate a record cycle
			
			sdtemp_cnt++;
			
			/*******************************insert sd cmd during sample cycles(back insertion)***************************************/
			if(sd_cycle == SD_CYCLE_SOTRE-1)  
			{
				/*******************************start DMA***************************************/		
				TransferError=SD_OK;
				StopCondition=1;			//多块写,需要发送停止传输指令 
				TransferEnd=0;				//传输结束标置位，在中断服务置1
				SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
				SD_DMA_Config((u32*)(u8*)&tmpbuf_sd[sd_dma_num][0],SD_WRITE_SECETOR_NUM*blksize,DMA_DIR_MemoryToPeripheral);		//SDIO DMA config and start DMA
				SDIO->DCTRL|=1<<3;								//SDIO DMA使能.

				block_num = block_num+SD_WRITE_SECETOR_NUM;
				sdtemp_cnt=0;
				sd_dma_num = (sd_dma_num+1)%2;  // switch DMA mem
				wait_until_xus();  // wait until 50us
				//t14 = TIM2->CNT;
				t13 = real_sd_cycle;
				//t15 = TIM2->CNT;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-3)
			{
			/*******************************check cmd***************************************/	
				errorstatus=CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
	
				if(errorstatus!=SD_OK)return errorstatus;
			/*******************************config data***************************************/	
				SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	//blksize, 控制器到卡	
				SDIO_DataInitStructure.SDIO_DataLength= SD_WRITE_SECETOR_NUM*blksize ;
				SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
				SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
				SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
				SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
				SDIO_DataConfig(&SDIO_DataInitStructure);
				wait_until_xus();  // wait until 50us
				t12 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-5)
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					/*******************************check cmd***************************************/	
					errorstatus=CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);//等待R1响应 
					
					if(errorstatus!=SD_OK)return errorstatus;		
				}
				/*******************************send CMD35***************************************/	
				SDIO_CmdInitStructure.SDIO_Argument =addr;	//发送CMD25,多块写指令,短响应 	  
				SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
				SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
				SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
				SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
				SDIO_SendCommand(&SDIO_CmdInitStructure);	
				wait_until_xus();  // wait until 50us
				t11 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-7)
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					/*******************************check cmd***************************************/	
					errorstatus=CmdResp1Error(SD_CMD_APP_CMD);		//等待R1响应 
					
					if(errorstatus!=SD_OK)return errorstatus;		
					/*******************************send CMD23***************************************/	
					SDIO_CmdInitStructure.SDIO_Argument = SD_WRITE_SECETOR_NUM;		//发送CMD23,设置块数量,短响应 	 
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);
				}
				
				wait_until_xus();  // wait until 50us
				t10 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-9)
			{
			/*******************************check dataconfig and cmd***************************************/	
				errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应  
		
				if(errorstatus!=SD_OK)return errorstatus;   	//响应错误
			/*******************************send ACMD55***************************************/	
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
				//提高性能
					SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA<<16;		//发送ACMD55,短响应 	
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
				}
				wait_until_xus();  // wait until 50us
				t9 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-11)
			{
				SDIO_CmdInitStructure.SDIO_Argument = blksize;//发送CMD16+设置数据长度为blksize,短响应 	
				SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
				SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
				SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
				SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
				SDIO_SendCommand(&SDIO_CmdInitStructure);	
				
				wait_until_xus();  // wait until 50us
				t8 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-13)
			{
				SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)
				
				SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;//清除DPSM状态机配置
				SDIO_DataInitStructure.SDIO_DataLength= 0 ;
				SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
				SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
				SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
				SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
				SDIO_DataConfig(&SDIO_DataInitStructure);
				if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
				addr = block_num; //set sd card addr
				addr<<=9;
				if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
				{
					blksize=512;
					addr>>=9;
				}    
				power=convert_from_bytes_to_power_of_two(blksize);	
				wait_until_xus();  // wait until 50us
				t7 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-15)
			{
		/*******************************check card status***************************************/	
				vu32 respR1 = 0, status = 0;  
				t17 = TIM2->CNT;
				status=SDIO->STA;
				while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//等待操作完成
					if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)			//CRC检测失败
				{  
					SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);	//清除错误标记
					return SD_CMD_CRC_FAIL;
				}
					if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)			//命令超时 
				{
					SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//清除错误标记
					return SD_CMD_RSP_TIMEOUT;
				}
				if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
				SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
				respR1=SDIO->RESP1;
				cardstate=(u8)((respR1>>9)&0x0000000F);
				t18 = TIM2->CNT;
				while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
				{
					errorstatus=IsCardProgramming(&cardstate);
				}
				t19 = TIM2->CNT;
				wait_until_xus();  // wait until 50us
				t20 = TIM2->CNT;
				t6 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-25)
			{
				/*******************************check card status***************************************/	
				SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; //卡相对地址参数
				SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;//发送CMD13 	
				SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
				SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
				SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
				SDIO_SendCommand(&SDIO_CmdInitStructure);	
				wait_until_xus();  // wait until 50us
				t5 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-55)
			{
				timeout=SDIO_DATATIMEOUT;
				while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;  // wait sd receive
				if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
				if(TransferError!=SD_OK)return TransferError;	
			/*******************************清除所有sd标记 ***************************************/					
				SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
				
				wait_until_xus();  // wait until 50us
				t4 = real_sd_cycle;
			}
			else if(sd_cycle == SD_CYCLE_SOTRE-56)
			{
				if (StopCondition==1&&TransferEnd==1)  // TransferEnd will set to 0 in interrupt 
				{  
					TransferError=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
					if(TransferError!=SD_OK)return TransferError;	
				}
				wait_until_xus();
				t3 = real_sd_cycle;	
			}
			else if(sd_cycle ==SD_CYCLE_SOTRE-58)
			{
				if (StopCondition==1&&TransferEnd==1)  // TransferEnd will set to 0 in interrupt 
				{  
					SDIO_CmdInitStructure.SDIO_Argument =0;//发送CMD12+结束传输 	  
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
				}
				wait_until_xus();  // wait until 50us
				t2 = real_sd_cycle;		
			}
			if(sd_cycle == SD_CYCLE_SOTRE-60)  // only can run below code after 80% sample cycle finish
			{
				while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待DMA传输完成 , should no use time 
				if(timeout==0)	 								//超时
				{									  
					SD_Init();	 					//重新初始化SD卡,可以解决写入死机的问题
					return SD_DATA_TIMEOUT;			//超时	 
				}
				wait_until_xus();  // wait until 50us
				t1 = real_sd_cycle;	
			}
			else
			{
				wait_until_xus();  // wait until 50us
			}
			sd_cycle = (sd_cycle+1)%SD_CYCLE_SOTRE;  // set next cycle number
				
			
				
			

			
//			if(sdtemp_cnt == 33*62)  //write 1 sector one time, 8*512-66*62=4byte left at the end of sector 
//			{
//				u32 time88,time99 = 0;
//						
//			/*******************************等待上一次DMA传输完成 ***************************************/
//							
//							 /***********************等待上一次DMA传输完成 ********************************/
//							while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待传输完成 
//							if(timeout==0)	 								//超时
//							{									  
//								SD_Init();	 					//重新初始化SD卡,可以解决写入死机的问题
//								return SD_DATA_TIMEOUT;			//超时	 
//							}
//							timeout=SDIO_DATATIMEOUT;
//							while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
//							if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
//							if(TransferError!=SD_OK)return TransferError;	
//			        /*******************************清除所有sd标记 *******************************/		
//							SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
//							
//			/*******************************check if sd programmig ***************************************/		
//						time88 = TIM2->CNT;							
//							errorstatus=IsCardProgramming(&cardstate);
//							while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
//							{
//								
//								errorstatus=IsCardProgramming(&cardstate);
//							}
//			/*******************************start sd DMA with no wait***************************************/	
//		
//							time99 = TIM2->CNT;								
//							SD_WriteDisk_nowait((u8*)&tmpbuf_sd[sd_dma_num][0],block_num,8); 
//							block_num = block_num+1;
//							sdtemp_cnt=0;
//							sd_dma_num = (sd_dma_num+1)%2;  // switch DMA mem
//							time3 = TIM2->CNT;
//			}
		}
		
//	if(sign10)  // add intan character send function
//		{
//				int j = 0;
//				for(j = 0; j < 5; j++)
//				{
//					SPI_CS_LOW();
//					
//					SPI_SendHalfWord(SPI_TX_intan[j]);

//					SPI_CS_HIGH();
//					
//					Usart_SendHalfWord(USART6,SPI_I2S_ReceiveData(SPI1));
//				}
//		}
//		if(sign11) 
//		{
//			uint16_t m = 0;
//			for(m = 0; m < 65535; m++)
//			{
//				//tmpbuf_sd[sdtemp_cnt] = m;
//				Usart_SendHalfWord(USART6, m);
//				//sdtemp_cnt++;
//				//if(sdtemp_cnt == 4096)
//				//{
//					//SD_WriteDisk((u8*)tmpbuf_sd,block_num,16);
//					//block_num = block_num+16;			
//					//sdtemp_cnt=0;
//				//}
//			}
//				
//		}
		
}

}



