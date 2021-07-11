#include <8051.h>
#include <stdio.h>
#include "Keypad4x4.h"
//#define TIMER_VAL 3036
int count = 10;
int led=0xff;
int record[29];
int r_count=0;
int record_check=0;
int i=0;
int sound=0;
int new_sound_check=0;
int time_check=0;

__code unsigned number[16] =
{
    0xc0,   //0
    0xf9,   //1
    0xa4,   //2
    0xb0,   //3
    0x99,   //4
    0x92,   //5
    0x82,   //6
    0xf8,   //7
    0x80,   //8
    0x90,   //9
    0x08,   //A
    0x00,   //B
    0x46,   //C
    0x40,   //D
    0x06,   //E
    0x0e,   //F
};

__code unsigned tone[10] =
{
    0,
	65536 - (1000000 / (262 * 2)), // C  0 
	65536 - (1000000 / (294 * 2)), // D  2
	65536 - (1000000 / (330 * 2)), // E  4
	65536 - (1000000 / (349 * 2)), // F  5
	65536 - (1000000 / (392 * 2)), // G  7
	65536 - (1000000 / (440 * 2)), // A  9
	65536 - (1000000 / (494 * 2)), // B  11

	65536 - (1000000 / (523 * 2)), // C  12
	65536 - (1000000 / (587 * 2)), // D  14
	//65536 - (1000000 / (659 * 2)), // E  16
	//65536 - (1000000 / (698 * 2)), // F  17
	//65536 - (1000000 / (784 * 2)), // G  19
};

__code unsigned TIMER_VAL[3] =
{
    3036,   //0.625
    15536,  //0.5
    35536,  //0.3
};

void timer_isr(void) __interrupt(1) __using(1)
{
    if(led!=0xff)
    {
        if(led==13)     //切換節拍速度 -> D
        {
            if(time_check==0)
            {
                time_check++;
            }
            else if(time_check==1)
            {
                time_check++;
            }
            else
            {
                time_check=0;
            }
            i=0;
            led=12;
        }    
    }
    TH0 = TIMER_VAL[time_check] >> 8;
	TL0 = TIMER_VAL[time_check] & 0xff;
    count++;
}
void timer_isr2(void) __interrupt(3) __using(2)
{
    if(led!=0xff)
    {
        if(led>=0 && led<=9)    //0~9 -> 0~9
        {
            sound=led;
            if(record_check==1)   //錄音開啟
            {
                if(new_sound_check==1)    
                {
                    if(led==0)
                    {
                        record[r_count]=-1;
                    }
                    else
                    {
                        record[r_count]=sound;
                    }
                    r_count++;     
                    new_sound_check=0;
                }
            } 
            TH1 = tone[sound] >> 8;
            TL1 = tone[sound] & 0xff;
            if(led!=0)
            {
                P2 = ~P2;
            }
        }
        else if(led == 10)      //錄音開啟on -> A
        {
            if(record_check == 0)
            {
                record_check = 1;
            }
        }
        else if(led==11)        //錄音關閉off -> B
        {
            i=-1;
            if(record_check == 1)
            {
                record_check = 0;
            }
        }
        else if(led == 12)      //播放錄音 -> C
        {
            if(r_count!=0 && record_check ==0)
            {
                TR0=1;
                if(count == 10)
                { 
                    count=0;
                    i++;
                    if(i==r_count)
                    {
                        i=0;
                    }
                }
                sound = record[i];
                if(sound!=-1)
                {
                    TH1 = tone[sound] >> 8;
                    TL1 = tone[sound] & 0xff;
                    P2 = ~P2;
                }
            }
        }        
        else if(led == 14)      //reset -> E
        {
            for(int j=0;j<=r_count;j++)
            {
                record[j] = -1;
            }
            r_count=0;
            record_check = 0;
            i=0;
            new_sound_check=0;
            TR0=0;
            count=10;
            led=0xff;
            time_check=0;
        }    
    }
}

void LED_Display(int led) 
{
    P1 = number[led];
    for (int j = 0; j < 1024; j++) {
    }	
} 

void main() {
    int flag = 0;
    int x = 0;
    int check = 0;
    int debounce = 1;
    TMOD = 0x11;
	TH0 = TIMER_VAL[time_check] >> 8;
	TL0 = TIMER_VAL[time_check] & 0xff;
	TR0 = 0; // start Timer 0
	ET0 = 1; // enable Timer 0 overflow interrupt
	EA = 1; //enable all interrupt
	TR1 = 1;
	ET1 = 1;

    P1 = 0xff;
    P0 = 0xff;
    while (1) 
    {
        if (check >= 0) 
        {
            do 
            {
                x = Keypad4x4();
                if (x >= 0 && x <= 15) 
                {
                    if(debounce!=0) 
                    {
                        flag = x;
                    }
                    debounce = 0;
                } 
                else 
                {
                    debounce++;
                    break;
                }
            } while (x >= 0 && x <= 15);
        }
        check++;
        if (check > 0) 
        {
            if (flag >= 0 && flag <= 15 && debounce == 1) 
            {
                led = 0x00 + flag;
                new_sound_check=1;
            }
            check = -10;
        }
        LED_Display(led);
    }
}