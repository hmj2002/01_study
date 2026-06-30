#ifndef __BSP_EC12_H__
#define __BSP_EC12_H__

#include "gpio_cfg.h"
#include "EXTI.h"
//////////////////// EC12 旋钮 ////////////////////
/*  接线说明
交互板           GD32F407
        A  ===>  PD11
        B  ===>  PD13
        D  ===>  PD15
       3v3 ===>  3v3
       GND ===>	 GND
*/

//// 外部中断实现
#define EC12_A_RCU			RCU_GPIOD
#define EC12_A_PIN			GPIOD, GPIO_PIN_11
#define EC12_A_STATE()		gpio_input_bit_get(EC12_A_PIN)
#define EC12_A_ON_TRIG()	EXTI11_on_trig() // 外部中断11回调函数

#define EC12_B_RCU			RCU_GPIOD
#define EC12_B_PIN			GPIOD, GPIO_PIN_13
#define EC12_B_STATE()		gpio_input_bit_get(EC12_B_PIN)

#define EC12_D_RCU			RCU_GPIOD
#define EC12_D_PIN			GPIOD, GPIO_PIN_15
#define EC12_D_STATE()		gpio_input_bit_get(EC12_D_PIN)
#define EC12_D_ON_TRIG()	EXTI15_on_trig() // 外部中断15回调函数

//// 中断实现 
void EC12_init();

#define EC12_USE_DOWN	1
#define EC12_USE_UP		1
void EC12_on_down();
void EC12_on_up();

#define EC12_USE_RIGHT	1
#define EC12_USE_LEFT	1
void EC12_on_turn_right(); // 顺时针，在时钟的上半部分，指针是向右移动的
void EC12_on_turn_left();  // 逆时针，在时钟的上半部分，指针是向左移动的

#endif