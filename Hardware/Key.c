#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Get_Key1num(void)
{
	uint8_t Keynum = 0;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)//up
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0);
		Delay_ms(20);
		Keynum = 1;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)//enter
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0);
		Delay_ms(20);
		Keynum = 2;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)//down
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0);
		Delay_ms(20);
		Keynum = 3;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)//left
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0);
		Delay_ms(20);
		Keynum = 4;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)//right
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0);
		Delay_ms(20);
		Keynum = 5;
	}
	return Keynum;
	
}
