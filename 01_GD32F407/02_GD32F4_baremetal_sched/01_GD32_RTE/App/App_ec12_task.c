#include "App.h"

static int cnt=100;
static uint8_t EC12_flag=0;

void EC12_on_turn_right() { // 顺时针，在时钟的上半部分，指针是向右移动的
    if(EC12_flag==0) return;
    cnt+=10;		
    if(cnt>190) cnt=0;	
    LED188_charge(1);
    LED188_percent(0);
    LED188_set_num(cnt);
}

void EC12_on_turn_left() {
    if(EC12_flag==0) return;
	  cnt-=10;
	  if(cnt<0) cnt=190;
	  LED188_charge(0);
    LED188_percent(1);
    LED188_set_num(cnt);


}

void EC12_on_down() {

    EC12_flag=!EC12_flag;

    EC12_flag==1? LED188_set_num(cnt):LED188_stop();

}

