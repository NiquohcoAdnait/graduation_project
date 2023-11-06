#ifndef _ESP8266_H_
#define _ESP8266_H_


#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

void ESP8266_Init(void);
void ESP8266_Clear(void);
void ESP8266_SendData(uint8_t *data, uint16_t len);
uint8_t *ESP8266_GetIPD(uint16_t timeOut);

#endif
