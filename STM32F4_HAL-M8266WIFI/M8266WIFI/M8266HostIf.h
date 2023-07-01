/********************************************************************
 * M8266HostIf.h
 * .Description
 *     header file of M8266WIFI Host Interface 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#ifndef _M8266_HOST_IF_H_
#define _M8266_HOST_IF_H_
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h" //这个看最后要不要加！

//发送和接收16位数据的函数，使用SPI2和Intan连接
#define CONVERT0                0x0000 //ADC CONVERT COMMAND
#define CONVERT1                0x0100
#define CONVERT2                0x0200
#define CONVERT3                0x0300
#define CONVERT4                0x0400
#define CONVERT5                0x0500
#define CONVERT6                0x0600
#define CONVERT7                0x0700
#define CONVERT8                0x0800
#define CONVERT9                0x0900
#define CONVERT10               0x0a00
#define CONVERT11               0x0b00
#define CONVERT12               0x0c00
#define CONVERT13               0x0d00
#define CONVERT14               0x0e00
#define CONVERT15               0x0f00
#define CONVERT16               0x1000
#define CONVERT17               0x1100
#define CONVERT18               0x1200
#define CONVERT19               0x1300
#define CONVERT20               0x1400
#define CONVERT21               0x1500
#define CONVERT22               0x1600
#define CONVERT23               0x1700
#define CONVERT24               0x1800
#define CONVERT25               0x1900
#define CONVERT26               0x1a00
#define CONVERT27               0x1b00
#define CONVERT28               0x1c00
#define CONVERT29               0x1d00
#define CONVERT30               0x1e00
#define CONVERT31               0x1f00
#define CONVERT32               0x2000 //auxiliary command
#define CONVERT33               0x2100
#define CONVERT34               0x2200


#define SPI2_CS_GPIO_PORT    GPIOB
#define SPI2_CS_PIN          GPIO_PIN_12
#define SPI2_CS_HIGH() HAL_GPIO_WritePin(SPI2_CS_GPIO_PORT, SPI2_CS_PIN, GPIO_PIN_SET)
#define SPI2_CS_LOW() HAL_GPIO_WritePin(SPI2_CS_GPIO_PORT, SPI2_CS_PIN, GPIO_PIN_RESET)  // 拉低CS引脚


void M8266HostIf_Init(void);
void SPI2_Init(u32 x);


uint16_t SPI_SendHalfWord(SPI_HandleTypeDef *hspi2, uint16_t HalfWord);


void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler);
void M8266HostIf_usart_txd_data(char* data, u16 len);
void M8266HostIf_usart_txd_string(char* str);
void M8266HostIf_USART_Reset_RX_BUFFER(void);
u16 M8266HostIf_USART_RX_BUFFER_Data_count(void);
u16 M8266HostIf_USART_get_data(u8* data, u16 len);

#endif

