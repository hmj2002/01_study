#ifndef __APP_H_
#define __APP_H_

#include "gpio_cfg.h"

//驱动头文件
#include "bsp_led188.h"
#include "bsp_adkey.h"

extern uint8_t LED188_flag;

static void App_init(){

//任务函数初始化
//App_Debug_init();
	
	LED188_init();
	ADKey_init();
}

//业务函数声明
void App_Debug_task();

void App_Debug_LED188_test_task();

void App_LED188_1_199_task();

void App_ADKey_task();
#endif