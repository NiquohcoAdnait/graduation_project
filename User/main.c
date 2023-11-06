#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "Key.h"
#include "CHsensor.h"
#include "Flowsensor.h"
#include "esp8266.h"
#include "Rtcclk.h"
#include "usart.h"
#include "onenet.h"
#include "OLED.h"
#include "Delay.h"
#include "settime.h"
#include "Show.h"
#include "cJSON.h"

Control Controlsignal;
timer time;
uint8_t pubflag;
uint8_t pop;
uint32_t kuwei;
uint32_t tianchi;
uint16_t humidnes;
uint32_t liacount;
uint32_t setlia;
uint8_t flag;
uint32_t stop_flag;
const int8_t pubbuf[256];	//上传数据
const int8_t* mqttdataptr; 		//MQTT数据包返回指针
uint16_t humidness_data[24] = 
{
	19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,
	19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,19.76,
};

void perControl(void);
void upload(void);
void download(void);
void Hardware_Init(void);

int main(void) 
{
	const char *topics[] = {"awsyssub"};
	Hardware_Init();
	while(Controlsignal.flag_Init != 1)
	{	
		Controlsignal.flag_Init = DevLink();
		Delay_ms(200);
	}

	//有网络连接自动获取数据
	if(Controlsignal.flag_Init == 1)
	{
		Subscribe(topics, 1);
		download();
		Controlsignal.set = 0;
		Controlsignal.ControlMode = 0;
	}
	//无网络连接，手动设置必要数据
	else
	{
		showdata(&time,&Controlsignal,&setlia,&liacount);//初次显示
		while(Controlsignal.flag_Init != 1)			//初次载入，设置时间等数据
		{
			settime(&time,&Controlsignal);			//设置灌溉时间
			setliang(&setlia,&Controlsignal);		//设置灌溉水量
			time.timecount = time.nowh*3600 + time.nowmin*60;
			showsetdata(time,Controlsignal,setlia,liacount);
		};
	}

	tianchi = 24.7;
	kuwei = 7.2;

	while(1)		//主循环
	{	

		humidnes = gethumidnes();		//获取土壤湿度
		perControl();					//外设控制函数

		if(Controlsignal.ControlMode == 0)	
		{
			download();
			show(time,liacount,pop,humidnes);
			if(pubflag == 1)
			{
				upload();
				pubflag = 0;
			}
			if(time.nowh == 0 && setlia == 0)
				liacount = 0;
		}
		else	
		{
			if(Controlsignal.set == 1)						//进入设置模式
			{
				showdata(&time,&Controlsignal,&setlia,&liacount);	//oled显示
				while(Controlsignal.set != 0)
				{
					setnowtime(&time,&Controlsignal);		//当前时间
					settime(&time,&Controlsignal);		    //设置灌溉时间
					setliang(&setlia,&Controlsignal);		//设置灌溉水量
					time.timecount = time.nowh*3600 + time.nowmin*60;
					showsetdata(time,Controlsignal,setlia,liacount);
				};
			}
			showdata(&time,&Controlsignal,&setlia,&liacount);	//oled显示
		}
	}
}

//核心控制
void perControl()
{	
	uint16_t diff = tianchi*0.2;
	uint16_t mid = humidness_data[time.nowh];
	uint16_t xiu = pop;

	if(setlia != 0 && time.nowh == time.h && time.nowmin == time.min)
	{
		stop_flag = liacount/450 + setlia;
		GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET);
		flag = 1;
	}
	if(flag == 1)
	{
		if((liacount/450) >= stop_flag)
		{
			setlia = 0;
			flag = 0;
			GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET);
		}
		
	}
	else
	{
		if((humidnes <= mid - diff - xiu)||(humidnes <= kuwei))
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET);
			Controlsignal.out = 1;
		}
		if((humidnes >= mid + diff)||(humidnes >= tianchi))
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET);
			Controlsignal.out = 0;
		}
	}
}

//外设初始化
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	        
	Rtcclk_Init();
	Usart_Init(115200);
	OLED_Init();
	Key_Init();
	Flowsensor_Init();
	CHsensor_Init();
	ESP8266_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	Controlsignal.ControlMode = 1;
	Controlsignal.set = 1;
	Controlsignal.setMode = 1;
	Controlsignal.setCount = 1;
 
}

//数据上传
void upload(void)		
{
	//sprintf(pubbuf,"{\"土壤湿度\":%d,\"灌溉水量\":%d}", humidnes,liacount/450);

	sprintf(pubbuf,"{\"hum\":%d,\"flow\":%d,\"gate\":%d}",humidnes,liacount/450,Controlsignal.out);
	Publish("awsys/pub",pubbuf);
	ESP8266_Clear();
}

void download(void)
{

	mqttdataptr = ESP8266_GetIPD(2);
	if(mqttdataptr != NULL)
	{
		OLED_ShowString(1,1,"GET DATA");
		Delay_ms(500);
		RevPro(mqttdataptr,&time,&setlia,&pop,&kuwei,&humidness_data);
	}
	Delay_ms(10);

}

void EXTI0_IRQHandler(void)
{
	if(Controlsignal.set == 1)
		Controlsignal.add++;	
	if(Controlsignal.set == 0)
	{
		Controlsignal.set = 1;
		Controlsignal.setMode = 2;
		Controlsignal.ControlMode = 1;
		Controlsignal.setCount = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void)
{
	if(Controlsignal.set == 1)
		Controlsignal.sub++;
	EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI2_IRQHandler(void)
{
	if(Controlsignal.set == 1)
	{
		if(Controlsignal.setCount == 4)
		{
			if(Controlsignal.setMode == 3)
			{
				Controlsignal.setCount = 0;
				Controlsignal.setMode = 0;
				Controlsignal.set = 0;
				Controlsignal.ControlMode = 0;
				Controlsignal.flag_Init = 1;
			}
			else
			{
				Controlsignal.setCount = 1;
				Controlsignal.setMode++;
			}
		}
		else
			Controlsignal.setCount++;
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
	
	if(Controlsignal.set == 1)
	{
		if(Controlsignal.setCount == 1)
		{
			if(Controlsignal.setMode == 2 && Controlsignal.flag_Init == 1)
			{
				Controlsignal.setCount = 0;
				Controlsignal.setMode = 0;
				Controlsignal.set = 0;
				Controlsignal.ControlMode = 0;
			}
			else
			{
				Controlsignal.setCount = 4;
				Controlsignal.setMode--;
			}
		}
		else
			Controlsignal.setCount--;
	}

	EXTI_ClearITPendingBit(EXTI_Line3);
}

void RTC_IRQHandler(void)
{
	if(Controlsignal.set == 0)
	{
		time.timecount++;
		if(time.timecount%5 == 0)
			pubflag = 1;

	}
	time.nowh = (time.timecount/3600)%24;
	time.nowmin = (time.timecount/60)%60;
	humidnes = gethumidnes();
	RTC_WaitForLastTask();
	RTC_ClearITPendingBit(RTC_IT_SEC);
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		liacount++;
	}

	EXTI_ClearITPendingBit(EXTI_Line15);
}
