#ifndef __BSP_NIXIE_H__
#define __BSP_NIXIE_H__

#include "gpio_cfg.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

// 声明宏变量
#define NIX_DI_rcu		RCU_GPIOD 	// 数据输入
#define NIX_DI_port		GPIOD	
#define NIX_DI_pin    GPIO_PIN_0

#define NIX_SCK_rcu		RCU_GPIOD
#define NIX_SCK_port	GPIOD     	// 移位寄存器
#define NIX_SCK_pin    GPIO_PIN_4

#define NIX_RCK_rcu		RCU_GPIOD
#define NIX_RCK_port	GPIOD	      // 锁存寄存器
#define NIX_RCK_pin   GPIO_PIN_1
// 初始化
void Nixie_init();

// num: 控制显示的什么内容
// idx: 控制显示哪几个显示
void Nixie_show(u8 num, u8 idx);

// num 对应的内容在数组的位置(索引)，配合自定义码表
// 码表https://www.yuque.com/mikejiangsky/ca1cyy/sk0cxccrvps7gnuv#ZxeBQ
// 除了0~9，刚好和下标一致，其它的内容，需要查表
// idx  显示在屏幕上的位置(1 -> 8)，只有1个数码管显示
void Nixie_display(u8 num, u8 idx);

void Nixie_clear(); // 灯灭

#endif