#include "bsp_current.h"


// 初始化
void Current_init(){
	printf("Current_init\n");
}

// 获取电流
float Current_get(){
		uint16_t adc=ADC0_get(2); 
	  float vol=adc*3.3/4096;
	  return (vol-1.65)/5;
}