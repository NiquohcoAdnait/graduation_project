#ifndef _ONENET_H_
#define _ONENET_H_

_Bool DevLink(void);
void Subscribe(const char *topics[], unsigned char topic_cnt);
void Publish(const char *topic, const char *msg);
void RevPro(unsigned char *cmd,timer *time,uint32_t *setlia,uint8_t *pop,uint8_t *ku,uint16_t *humiddata[24]);

#endif
