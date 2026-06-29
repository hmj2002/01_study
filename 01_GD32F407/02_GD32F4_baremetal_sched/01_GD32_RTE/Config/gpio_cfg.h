#ifndef __GPIO_CFG_H__
#define __GPIO_CFG_H__

#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"
#include <stdio.h>
#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#define USE_SPI0_soft_hard 1  //如果为1：硬实现    0：软实现

/**********************************************************
 * @brief   输出配置 
 * @param 	rcu  	时钟端口  	RCU_GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	port  	引脚端口  	GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	pin  	引脚  	  	GPIO_PIN_x(x=0..15)
 * @param 	otype  	输出模式  	GPIO_OTYPE_PP 推挽; GPIO_OTYPE_OD 开漏
 * @param 	flag  	电平状态  	0 配置低电平  1 配置高电平  2 不配置电平，默认状态
 * @return  
 **********************************************************/
static inline void GPIO_output(rcu_periph_enum rcu, uint32_t port, uint32_t pin, uint8_t otype, uint8_t flag) {
	rcu_periph_clock_enable(rcu);
	// 对于推挽输出而言，这个上拉是不起作用的，对于开漏而言，才会起作用
	gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, pin);
	gpio_output_options_set(port, otype, GPIO_OSPEED_MAX, pin);
	
	if (flag == 0) gpio_bit_reset(port, pin);
	else if (flag == 1) gpio_bit_set(port, pin);
}

/**********************************************************
 * @brief   输入配置 
 * @param 	rcu  			时钟端口  	RCU_GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	port  			引脚端口  	GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	pin  			引脚  	  	GPIO_PIN_x(x=0..15)
 * @param 	pull_up_down  	上下拉模式  	GPIO_PUPD_NONE 浮空
										GPIO_PUPD_PULLUP 上拉
										GPIO_PUPD_PULLDOWN 下拉
 * @return  
 **********************************************************/
static inline void GPIO_input(rcu_periph_enum rcu, uint32_t port, uint32_t pin, uint32_t pull_up_down) {
	rcu_periph_clock_enable(rcu);
	gpio_mode_set(port, GPIO_MODE_INPUT, pull_up_down, pin);
}

/**********************************************************
 * @brief   模拟输入配置 
 * @param 	rcu  			时钟端口  	RCU_GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	port  			引脚端口  	GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	pin  			引脚  	  	GPIO_PIN_x(x=0..15)
 * @param 	pull_up_down  	上下拉模式  	GPIO_PUPD_NONE 浮空
										GPIO_PUPD_PULLUP 上拉
										GPIO_PUPD_PULLDOWN 下拉
 * @return  
 **********************************************************/
static inline void GPIO_analog(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
	rcu_periph_clock_enable(rcu);
	gpio_mode_set(port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin);
}

 /**********************************************************
 * @brief   复用输出配置 
 * @param 	rcu  	时钟端口  	RCU_GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	port  	引脚端口  	GPIOx(x = A,B,C,D,E,F,G,H,I)
 * @param 	pin  	引脚  	  	GPIO_PIN_x(x=0..15)
 * @param 	otype  	输出模式  	GPIO_OTYPE_PP 推挽; GPIO_OTYPE_OD 开漏
 * @param 	af_num  复用引脚  	GPIO_AF_x
 * @return  
 **********************************************************/
static inline void GPIO_output_af(rcu_periph_enum rcu,uint32_t port,uint32_t pin,uint8_t otype,uint32_t af_num){
	rcu_periph_clock_enable(rcu);
	gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin);
	gpio_output_options_set(port, otype, GPIO_OSPEED_MAX, pin);
  gpio_af_set(port,af_num,pin);
}
#endif