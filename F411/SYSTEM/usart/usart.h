#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2011/6/14
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define BLOCK_DMA_NUM_TX 2  // block for dma_to_uart
#define BLOCK_DMA_SIZE_TX 10 // dma send buffer
#define BUFFER_SIZE_RX 64  // dma receive buffer
extern u8 sign1,sign2,sign3,sign4,sign5,sign6,sign7,sign8,sign9,sign10, sign11, sign12;
extern u8 intan_cs_delay;
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern uint16_t tmpbuf_rev[BLOCK_DMA_NUM_TX][BLOCK_DMA_SIZE_TX]; // static mem for dma TX
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void USART6_DMA_Tx_Configuration(void);
void USART6_DMA_Rx_Configuration(void);
void USART6_Configuration(uint16_t bound);
void DMA_send_data(const uint16_t* data, uint32_t length);
#endif


