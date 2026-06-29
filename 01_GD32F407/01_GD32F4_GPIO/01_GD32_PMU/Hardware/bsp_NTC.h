#ifndef __BSP_NTC_H__
#define __BSP_NTC_H__

#include "gd32f4xx.h"
#include "gpio_cfg.h"
#include "systick.h"
void NTC_init();  // 初始化
int NTC_get_temperature(); // 获取温度


#endif