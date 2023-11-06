#include "stm32f10x.h"
#include "esp8266.h"
#include "main.h"
#include "onenet.h"
#include "mqttkit.h"
#include "usart.h"
#include "OLED.h"
#include "Delay.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>


#define PROID		"awsys"

#define AUTH_INFO	"88886666"

#define DEVID		"stm32"


extern unsigned char esp8266_buf[128];


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 0;
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(200);									//等待平台响应
		
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:OLED_ShowString(1,1,"LinkOK");status = 1;break;	//Tips:	连接成功
					
					case 1:OLED_ShowString(1,1,"error-1");break;		//连接失败：协议错误
					case 2:OLED_ShowString(1,1,"error-2");break;		//连接失败：非法的clientid
					case 3:OLED_ShowString(1,1,"error-3");break;		//连接失败：服务器失败
					case 4:OLED_ShowString(1,1,"error-4");break;		//连接失败：用户名或密码错误
					case 5:OLED_ShowString(1,1,"error-5");break;      	//连接失败：非法链接(比如token非法)
					
					default:OLED_ShowString(1,1,"error");break;			//连接失败：未知错误
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		//UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
		OLED_ShowString(1,1,"MQTT_PC Failed");
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
void Subscribe(const char *topics[], unsigned char topic_cnt)
{
		
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void RevPro(unsigned char *cmd,timer *time,uint32_t *setlia,uint8_t *pop,uint8_t *ku,uint16_t *humiddata[24])
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	uint8_t len = 0;
	cJSON *json, *Hmarrnum,*json_value,*Arrnumele;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				MQTT_DeleteBuffer(&mqttPacket);									//删包
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				json = cJSON_Parse(req_payload);
				if(!json)
				{
					OLED_Clear();
					OLED_ShowString(1,1,"JSON GET ERROR");
				}
				else
				{
					OLED_ShowString(1,1,"GET JSON");
					Delay_ms(1000);
					if((Hmarrnum = cJSON_GetObjectItem(json,"humidness")) != NULL)
					{
						len = cJSON_GetArraySize(Hmarrnum);
						for(int i = 0;i < len;i++)
						{
							Arrnumele = cJSON_GetArrayItem(Hmarrnum,i);
							humiddata[i] = Arrnumele->valueint;
						}
					}
					if((json_value = cJSON_GetObjectItem(json,"kuiwei")) != NULL)
						*ku = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"h")) != NULL)
						time->nowh = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"min")) != NULL)
						time->nowmin = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"sec")) != NULL)
					{
						time->timecount = json_value->valueint;
						time->timecount += time->nowh*3600 + time->nowmin*60;
					}
					if((json_value = cJSON_GetObjectItem(json,"pop")) != NULL)
						*pop = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"seth")) != NULL)
						time->h = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"setmin")) != NULL)
						time->min = json_value->valueint;
					if((json_value = cJSON_GetObjectItem(json,"lia")) != NULL)
						*setlia = json_value->valueint;
				}
				cJSON_Delete(json);
				MQTT_DeleteBuffer(&mqttPacket);	
			}
		
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			{
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;		
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
