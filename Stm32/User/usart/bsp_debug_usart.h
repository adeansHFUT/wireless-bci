#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>



#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
extern u8 sign1,sign2,sign3,sign4,sign5,sign6,sign7,sign8,sign9,sign10,sign11,sign12,sign13,sign14,sign15,sign16,sign17,sign18,sign19,sign20,sign21,sign22,sign23,sign24,sign25,sign26,sign27,sign28,sign29,sign30,sign31,sign32,sign33,sign34,sign35,sign36,sign37,sign38,sign39,sign40,sign41,sign42,sign43,sign44,sign45;		  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
#endif
