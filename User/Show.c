#include "stm32f10x.h"
#include "OLED.h"
#include "main.h"

void showdata(timer* time,Control* Controlsignal,uint32_t* setliang,uint32_t* liacount)
{
    int lia;
    lia = *liacount/450;

    OLED_ShowChinese(1,1,7);
    OLED_ShowChinese(1,2,8);
    OLED_ShowChinese(1,3,0);
    OLED_ShowChinese(1,4,1);
    OLED_ShowChinese(2,1,2);
    OLED_ShowChinese(2,2,3);
    OLED_ShowChinese(2,3,0);
    OLED_ShowChinese(2,4,1);
    OLED_ShowChinese(3,1,2);
    OLED_ShowChinese(3,2,3);
    OLED_ShowChinese(3,3,4);
    OLED_ShowChinese(3,4,5);
    OLED_ShowChinese(4,1,6);
    OLED_ShowChinese(4,2,2);
    OLED_ShowChinese(4,3,3);
    OLED_ShowChar(1,9,' ');
    OLED_ShowChar(2,9,' ');
    OLED_ShowChar(3,9,' ');
    OLED_ShowChar(4,7,' ');

    OLED_ShowNum(1,10,time->nowh,2);
    OLED_ShowNum(1,13,time->nowmin,2);
    OLED_ShowChar(1,12,':');
    OLED_ShowNum(2,10,time->h,2);
    OLED_ShowNum(2,13,time->min,2);
    OLED_ShowChar(2,12,':');
    OLED_ShowNum(3,10,*setliang,4);
    OLED_ShowChar(3,14,'L');
    OLED_ShowNum(4,8,lia,4);
    OLED_ShowChar(4,12,'L');
    OLED_ShowChar(4,13,' ');
}

void showsetdata(timer time,Control Controlsignal,uint32_t setliang,uint32_t liacount)
{
    int i,j,lia;
    lia = liacount/450;
    j = Controlsignal.setCount;
    if(Controlsignal.setCount >= 3 && Controlsignal.setMode != 3)
        j++;
    if(Controlsignal.set == 1)
    {
        for(i = 1 ; i < 6 ; i++ )
        {
            if(j == i)
                OLED_ShowMychar(Controlsignal.setMode + 1,i + 9,0);
            else
                OLED_ShowChar(Controlsignal.setMode + 1,i + 9,' ');
        }
        if(Controlsignal.setMode == 1)
        {
            OLED_ShowNum(1,10,time.nowh,2);
            OLED_ShowNum(1,13,time.nowmin,2);
            OLED_ShowChar(1,12,':');

            OLED_ShowNum(3,10,setliang,4);
            OLED_ShowChar(3,14,'L');

            OLED_ShowNum(4,8,lia,4);
            OLED_ShowChar(4,12,'L');
            OLED_ShowChar(4,13,' ');
            OLED_ShowChar(4,14,' ');
        }
        else if(Controlsignal.setMode == 2)
        {
            OLED_ShowNum(1,10,time.nowh,2);
            OLED_ShowNum(1,13,time.nowmin,2);
            OLED_ShowChar(1,12,':');

            OLED_ShowNum(2,10,time.h,2);
            OLED_ShowNum(2,13,time.min,2);
            OLED_ShowChar(2,12,':');

            OLED_ShowNum(4,8,lia,4);
            OLED_ShowChar(4,12,'L');
            OLED_ShowChar(4,13,' ');
            OLED_ShowChar(4,14,' ');
        }
        else
        {
            OLED_ShowNum(1,10,time.nowh,2);
            OLED_ShowNum(1,13,time.nowmin,2);
            OLED_ShowChar(1,12,':');

            OLED_ShowNum(2,10,time.h,2);
            OLED_ShowNum(2,13,time.min,2);
            OLED_ShowChar(2,12,':');

            OLED_ShowNum(3,10,setliang,4);
            OLED_ShowChar(3,14,'L');
        }
    }
}

void show(timer time,uint32_t liacount,uint8_t pop,uint16_t humidnes)
{
    OLED_ShowChinese(1,1,7);
    OLED_ShowChinese(1,2,8);
    OLED_ShowChinese(1,3,0);
    OLED_ShowChinese(1,4,1);

    OLED_ShowChinese(2,1,13);
    OLED_ShowChinese(2,2,14);
    OLED_ShowChinese(2,3,15);

    OLED_ShowChinese(3,1,9);
    OLED_ShowChinese(3,2,10);
    OLED_ShowChinese(3,3,11);
    OLED_ShowChinese(3,4,12);

    OLED_ShowChinese(4,1,6);
    OLED_ShowChinese(4,2,2);
    OLED_ShowChinese(4,3,3);

    OLED_ShowChar(1,9,' ');
    OLED_ShowChar(2,7,' ');
    OLED_ShowChar(2,8,' ');
    OLED_ShowChar(3,9,' ');
    OLED_ShowChar(3,10,' ');
    OLED_ShowChar(4,7,' ');
    OLED_ShowChar(4,8,' ');
    OLED_ShowChar(4,9,' ');

    OLED_ShowNum(1,10,time.nowh,2);
    OLED_ShowNum(1,13,time.nowmin,2);
    OLED_ShowChar(1,12,':');

    OLED_ShowNum(2,9,pop,4);
    OLED_ShowChar(2,13,'m');
    OLED_ShowChar(2,14,'m');

    OLED_ShowNum(3,11,humidnes,3);
    OLED_ShowChar(3,14,'%');

    OLED_ShowNum(4,10,liacount/450,4);
    OLED_ShowChar(4,14,'L');
}
