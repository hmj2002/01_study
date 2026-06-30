#ifndef __VOLTAGE_H__
#define __VOLTAGE_H__

#include "gpio_cfg.h"
#include "ADC0.h"
/*  接线说明
交互板           GD32F407
    (压)ADC ===>  PA3
       3v3 ===>  3v3
       GND ===>	 GND
*/

// 初始化
void Voltage_init();

// 获取电压
float Voltage_get();
    
#endif