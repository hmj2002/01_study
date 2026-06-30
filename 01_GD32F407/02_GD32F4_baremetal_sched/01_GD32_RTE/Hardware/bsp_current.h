#ifndef __BSP_CURRENT_H__
#define __BSP_CURRENT_H__

#include "gpio_cfg.h"
#include "ADC0.h"
// 初始化
void Current_init();

// 获取电流
float Current_get();
    
#endif