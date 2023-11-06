#ifndef __MAIN_H
#define __MAIN_H

typedef struct{
uint8_t min;
uint8_t h;
uint8_t nowmin;
uint8_t nowh;
uint32_t timecount;
}timer;

typedef struct{
	uint8_t flag_Init;
	uint8_t ControlMode;
	uint16_t out;
	uint8_t set;
	uint8_t add;
	uint8_t sub;
	uint8_t setMode;
	uint8_t setCount;
}Control;

#endif
