#ifndef __BSP_ADKEY_H__
#define __BSP_ADKEY_H__

#include "gpio_cfg.h"

//////////////////// ADKey ////////////////////
/*  接线说明
交互板           GD32F407
        K  ===>  PA2
       3v3 ===>  3v3
       GND ===>	 GND
*/
// IN2	PA2
#define ADKEY_RCU			RCU_GPIOA
#define ADKEY_PIN			GPIOA, GPIO_PIN_2

#define ADKEY_CHN			ADC_CHANNEL_2
#define ADKEY_SAMPLETIME	ADC_SAMPLETIME_84

void ADKey_init(); // 初始化
void ADKey_scan(); // 扫描

#endif