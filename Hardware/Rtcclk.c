#include "stm32f10x.h"

void Rtcclk_Init(void)
{
    uint32_t t = 0;
    NVIC_InitTypeDef NVIC_RtcInit;
    NVIC_RtcInit.NVIC_IRQChannel = RTC_IRQn;
    NVIC_RtcInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_RtcInit.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_RtcInit.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_RtcInit);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_DeInit();
    RCC_LSEConfig(RCC_LSE_ON);  //开启内部低速时钟
    //等待外部低速时钟稳定
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && t < 5000)
    {
        t++;
    }

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //选择LSE作为RTC时钟源
    RCC_RTCCLKCmd(ENABLE);      //使能RTC时钟

    RTC_WaitForSynchro();       //等待RTC寄存器时钟同步
    RTC_WaitForLastTask();      //等待RTC寄存器上一次操作完成

    RTC_EnterConfigMode();      //进入RTC配置模式
    RTC_WaitForLastTask();

    RTC_ITConfig(RTC_IT_SEC,ENABLE);    //使能RTC秒中断
    RTC_WaitForLastTask();

    RTC_SetPrescaler(32767);    //设置RTC预分频系数，周期为1s
    RTC_WaitForLastTask();
    
    RTC_SetCounter(0);          //设置RTC初始时间
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();       //退出RTC配置模式
}

