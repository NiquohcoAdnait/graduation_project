#include "stm32f10x.h"

void Flowsensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);

    EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure15;
	NVIC_InitStructure15.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure15.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure15.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure15.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure15);

}
