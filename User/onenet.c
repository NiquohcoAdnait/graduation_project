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
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 0;
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(200);									//�ȴ�ƽ̨��Ӧ
		
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:OLED_ShowString(1,1,"LinkOK");status = 1;break;	//Tips:	���ӳɹ�
					
					case 1:OLED_ShowString(1,1,"error-1");break;		//����ʧ�ܣ�Э�����
					case 2:OLED_ShowString(1,1,"error-2");break;		//����ʧ�ܣ��Ƿ���clientid
					case 3:OLED_ShowString(1,1,"error-3");break;		//����ʧ�ܣ�������ʧ��
					case 4:OLED_ShowString(1,1,"error-4");break;		//����ʧ�ܣ��û������������
					case 5:OLED_ShowString(1,1,"error-5");break;      	//����ʧ�ܣ��Ƿ�����(����token�Ƿ�)
					
					default:OLED_ShowString(1,1,"error");break;			//����ʧ�ܣ�δ֪����
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		//UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
		OLED_ShowString(1,1,"MQTT_PC Failed");
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void Subscribe(const char *topics[], unsigned char topic_cnt)
{
		
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����		
//==========================================================
void Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void RevPro(unsigned char *cmd,timer *time,uint32_t *setlia,uint8_t *pop,uint8_t *ku,uint16_t *humiddata[24])
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
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
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
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
			
		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
			
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
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
