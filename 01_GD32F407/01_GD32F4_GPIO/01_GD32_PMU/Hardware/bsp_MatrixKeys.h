#ifndef __BSP_MATRIX_KEYS__
#define __BSP_MATRIX_KEYS__

#include "gpio_cfg.h"
#include "systick.h"

#define MK_USE_DOWN 0
#define MK_USE_UP	 	0



// 只是做声明，用户使用的时候，相应开关打开，函数同时一定要定义
// 按下的回调函数
void MK_on_keydown(uint8_t row, uint8_t col);
// 抬起的回调函数
void MK_on_keyup(uint8_t row, uint8_t col);

// 初始化
void MK_init();

// 扫描按键
void MK_scan();

#endif