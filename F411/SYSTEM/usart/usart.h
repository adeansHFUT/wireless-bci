#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2011/6/14
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define BLOCK_DMA_NUM_TX 2  // block for dma_to_uart
#define BLOCK_DMA_SIZE_TX 10 // dma send buffer
#define BUFFER_SIZE_RX 64  // dma receive buffer
extern u8 sign1,sign2,sign3,sign4,sign5,sign6,sign7,sign8,sign9,sign10, sign11, sign12;
extern u8 intan_cs_delay;
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern uint16_t tmpbuf_rev[BLOCK_DMA_NUM_TX][BLOCK_DMA_SIZE_TX]; // static mem for dma TX
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void USART6_DMA_Tx_Configuration(void);
void USART6_DMA_Rx_Configuration(void);
void USART6_Configuration(uint16_t bound);
void DMA_send_data(const uint16_t* data, uint32_t length);
#endif


