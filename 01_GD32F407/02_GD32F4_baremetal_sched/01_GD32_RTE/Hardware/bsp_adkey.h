#ifndef __BSP_ADKEY_H__
#define __BSP_ADKEY_H__

#include "gpio_cfg.h"
#include "ADC0.h"
//////////////////// ADKey ////////////////////
/*  接线说明
交互板           GD32F407
        K  ===>  PA2
       3v3 ===>  3v3
       GND ===>  GND
*/
// IN2 -> PA2
#define ADKEY_RCU           RCU_GPIOA
#define ADKEY_PIN           GPIOA, GPIO_PIN_2

#define ADKEY_CHN           ADC_CHANNEL_2
#define ADKEY_SAMPLETIME    ADC_SAMPLETIME_84

void ADKey_init(void);
void ADKey_scan(void);

/* 当前稳定按下的按键：
   0: 无按键
   1: KEY1
   2: KEY2
   3: KEY3
*/
uint8_t ADKey_get_state(void);

/* 事件回调：可在其他 .c 文件中重写 */
void ADCKey_on_down(uint8_t i);          // 按下
void ADCKey_on_up(uint8_t i);            // 松开
void ADCKey_on_click(uint8_t i);         // 单击/短按
void ADCKey_on_long_click(uint8_t i);    // 长按
void ADCKey_on_double_click(uint8_t i);  // 双击（双击时不触发单击）
void ADCKey_on_long_repeat(uint8_t i);   // 长按连发（可选）

#endif
