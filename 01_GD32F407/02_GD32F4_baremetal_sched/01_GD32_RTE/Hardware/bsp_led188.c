#include "bsp_led188.h"

static uint32_t state = 0x0000; //都不亮，32位能标志32个灯，这边只用18位
// 个位
#define A3	(1L << 0)
#define B3	(1L << 1)
#define C3	(1L << 2)
#define D3	(1L << 3)
#define E3	(1L << 4)
#define F3	(1L << 5)
#define G3	(1L << 6)
// 十位
#define A2	(1L << 7)
#define B2	(1L << 8)
#define C2	(1L << 9)
#define D2	(1L << 10)
#define E2	(1L << 11)
#define F2	(1L << 12)
#define G2	(1L << 13)
// 百位 1
#define B1	(1L << 14)
#define C1	(1L << 15)
// 闪电
#define L1	(1L << 16) 
// %
#define L2	(1L << 17) 

static uint8_t table[] = {
    0x3F, //0
    0x06, //1
    0x5B, //2
    0x4F, //3
    0x66, //4
    0x6D, //5
    0x7D, //6
    0x07, //7
    0x7F, //8
    0x6F, //9
};

void LED188_init(){

 LED188_ALL_IN();
	
 LED188_set_num(167);
 LED188_charge(1);
 LED188_percent(1);
	
}

void LED188_display(){
	static uint8_t cnt=1;
	LED188_ALL_IN();
	if(cnt==1){
	LED188_PIN1_OUT(1);
	if(state & A3) LED188_PIN2_OUT(0);
	if(state & C3) LED188_PIN3_OUT(0);
	if(state & E3) LED188_PIN4_OUT(0);
	
	}else if(cnt==2){
	LED188_PIN2_OUT(1);
	if(state & B3) LED188_PIN1_OUT(0);
	if(state & A2) LED188_PIN3_OUT(0);
	if(state & C1) LED188_PIN4_OUT(0);
	if(state & L2) LED188_PIN5_OUT(0);
	
	}else if(cnt==3){
		LED188_PIN3_OUT(1);
	if(state & D3) LED188_PIN1_OUT(0);
	if(state & B2) LED188_PIN2_OUT(0);
	if(state & B1) LED188_PIN4_OUT(0);
	if(state & L1) LED188_PIN5_OUT(0);
	
	}else if(cnt==4){
		LED188_PIN4_OUT(1);
	if(state & F3) LED188_PIN1_OUT(0);
	if(state & D2) LED188_PIN2_OUT(0);
	if(state & C2) LED188_PIN3_OUT(0);

	
	}else if(cnt==5){
		LED188_PIN5_OUT(1);
	if(state & G3) LED188_PIN1_OUT(0);
	if(state & E2) LED188_PIN2_OUT(0);
	if(state & F2) LED188_PIN3_OUT(0);
	if(state & G2) LED188_PIN4_OUT(0);
	
	}
	cnt++;
	if(cnt>5) cnt=1;
}

void LED188_set_num(uint8_t num){// 数字
	if(num>199) return;
	
	state &= ~0xFFFF;
	
	uint8_t one= num %10;
	
	state|=table[one];
	
	if(num<10) return;
	
	 uint8_t ten= num /10 %10;
	
	 state|=table[ten]<<7;
	
	if(num<100) return;
	
	 state|=(B1|C1);
}

void LED188_charge(uint8_t enable){  // 闪电
	if(enable)  state|=L1;
	else state &=~L1;
	
}


void LED188_percent(uint8_t enable){ // %
  if(enable)  state|=L2;
	else state &=~L2;		
}

void LED188_stop(){ // %
state=0;
}
