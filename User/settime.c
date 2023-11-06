#include "stm32f10x.h"
#include "main.h"
#include "Show.h"

void setnowtime(timer* time,Control* Controlsignal)
{
	if(Controlsignal->setMode == 1)
	{
		switch(Controlsignal->setCount){
			case 1:
				if(Controlsignal->add == 1 && time->nowh <=13)
					time->nowh += 10;
				if(Controlsignal->sub == 1 && time->nowh >=10)
					time->nowh -= 10;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 2:
				if(Controlsignal->add == 1 && time->nowh <=22)
					time->nowh += 1;
				if(Controlsignal->sub == 1 && time->nowh >=1)
					time->nowh -= 1;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 3:
				if(Controlsignal->add == 1 && time->nowmin <=49)
					time->nowmin += 10;
				if(Controlsignal->sub == 1 && time->nowmin >=10)
					time->nowmin -= 10;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 4:
				if(Controlsignal->add == 1 && time->nowmin <=58)
					time->nowmin += 1;
				if(Controlsignal->sub == 1 && time->nowmin >=1)
					time->nowmin -= 1;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
		}
		time->timecount = 0;
	}
}

void settime(timer* time,Control* Controlsignal)
{
	if(Controlsignal->setMode == 2)
	{
		switch(Controlsignal->setCount){
			case 1:
				if(Controlsignal->add == 1 && time->h <=13)
					time->h += 10;
				if(Controlsignal->sub == 1 && time->h >=10)
					time->h -= 10;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 2:
				if(Controlsignal->add == 1 && time->h <=22)
					time->h += 1;
				if(Controlsignal->sub == 1 && time->h >=1)
					time->h -= 1;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 3:
				if(Controlsignal->add == 1 && time->min <=49)
					time->min += 10;
				if(Controlsignal->sub == 1 && time->min >=10)
					time->min -= 10;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 4:
				if(Controlsignal->add == 1 && time->min <=58)
					time->min += 1;
				if(Controlsignal->sub == 1 && time->min >=1)
					time->min -= 1;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
		}
	}
}

void setliang(uint32_t* lia,Control* Controlsignal)
{
	if(Controlsignal->setMode == 3)
	{
		switch(Controlsignal->setCount){
			case 1:
				if(Controlsignal->add == 1 && *lia <= 8999)
					*lia += 1000;
				if(Controlsignal->sub == 1 && *lia >=1000)
					*lia -= 1000;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 2:
				if(Controlsignal->add == 1 && *lia <= 9899)
					*lia += 100;
				if(Controlsignal->sub == 1 && *lia >=100)
					*lia -= 100;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 3:
				if(Controlsignal->add == 1 && *lia <= 9989)
					*lia += 10;
				if(Controlsignal->sub == 1 && *lia >=10)
					*lia -= 10;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
			case 4:
				if(Controlsignal->add == 1 && *lia <= 9998)
					*lia += 1;
				if(Controlsignal->sub == 1 && *lia >=1)
					*lia -= 1;
				Controlsignal->add = 0;
				Controlsignal->sub = 0;
			break;
		}
	}
}
