#include "led.h"

//On Nucleo-L452RE: LED2->GPIOA5

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();		// GPIOA
	
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_5;   // PA5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull=GPIO_PULLUP;
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
    LED_set(0, 0);
    LED_set(1, 0);
}

void LED_set(u8 led_no, u8 on)
{
	if(led_no!=0)
	{
		if(on) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		else   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
	else
	{
		if(on) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		else   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}
