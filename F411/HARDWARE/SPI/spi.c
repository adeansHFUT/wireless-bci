#include "spi.h"
#include "delay.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
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
//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Init(u16 x)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PB3复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PB4复用为 SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB5复用为 SPI1
  //GPIOFB3,4,5初始化设置
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
		/*!< 配置 SPI_SPI 引脚: CS */                 
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);

  /* 停止信号 FLASH: CS引脚高电平*/
  SPI_CS_HIGH();
	

 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	//SPI_InitStructure.SPI_BaudRatePrescaler = x;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

  delay_ms(1000);

/* 初始化芯片配置寄存器 */
	
	   SPI_CS_LOW(); delay_us( 50 );
	 
	   SPI_SendHalfWord(0x80fe);
	
     delay_us( 50 );SPI_CS_HIGH();
		
	   delay_us( 84000 );
	 
	 /* 初始化芯片配置寄存器0：ADC配置和放大器快速建立 */
	 	SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x80de);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* 初始化芯片配置寄存器1：传感器采样和ADC缓冲器偏置电流设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8120);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		/* 初始化芯片配置寄存器2：传感器采样和ADC缓冲器偏置电流设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8228);

		delay_us( 50 );SPI_CS_HIGH();
	
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器3：温度传感器和辅助数字输出设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8301);
	
		delay_us( 50 );SPI_CS_HIGH();
		
    delay_us( 500 );
	
	 	/* 初始化芯片配置寄存器4：ADC输出格式和DSP失调消除设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8480);

		delay_us( 50 );SPI_CS_HIGH();
		
    delay_us( 500 );
		
	 	/* 初始化芯片配置寄存器5：阻抗检测控制（未开启）设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8500);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器6：寄存器5的DAC设置（置零）设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8600);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* 初始化芯片配置寄存器7：同上置零设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8700);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器8：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x881e);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器9：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8985);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器10：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8a2b);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器11：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8b86);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );

	 	/* 初始化芯片配置寄存器12：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8c10);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );	
						
	 	/* 初始化芯片配置寄存器13：放大器带宽设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8dfc);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器14：放大器启动设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8eff);
    
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
	 	/* 初始化芯片配置寄存器15：放大器启动设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x8fff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* 初始化芯片配置寄存器16：放大器启动设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x90ff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		delay_us( 500 );
		
	 	/* 初始化芯片配置寄存器17：放大器启动设置 */
	 
	  SPI_CS_LOW(); delay_us( 50 );
	 
	  SPI_SendHalfWord(0x91ff);
	
		delay_us( 50 );SPI_CS_HIGH();
		
		/* 寄存器初始化后100us进行ADC校准,校准开始后需要额外9命令时钟 */
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
 
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u16 SPI_SendHalfWord(u16 HalfWord)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, HalfWord); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}








