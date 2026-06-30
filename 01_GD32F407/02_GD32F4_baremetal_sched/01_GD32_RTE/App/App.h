#ifndef __APP_H_
#define __APP_H_

#include "gpio_cfg.h"

//驱动头文件
#include "bsp_led188.h"
#include "bsp_adkey.h"
#include  "bsp_voltage.h"
#include "bsp_current.h"
#include "bsp_ec12.h"
extern uint8_t LED188_flag;

static void App_init(){

//任务函数初始化
//App_Debug_init();
	
	LED188_init();
	ADKey_init();
	Voltage_init();
	Current_init();
	EC12_init();
}

//业务函数声明
void App_Debug_task();//示例

void App_Debug_LED188_test_task();//188数码管

void App_LED188_1_199_task();//188数码管1~199循环

void App_ADKey_task();//AD按键

void App_Voltage_task();

void App_Current_task();


void App_LED188_1_199_add_task();//按键+任务

void App_LED188_199_1_sub_task();//按键-任务
#endif