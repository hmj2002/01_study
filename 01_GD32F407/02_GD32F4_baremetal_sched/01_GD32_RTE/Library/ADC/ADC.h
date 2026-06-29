#ifndef __ADC_H__
#define __ADC_H__

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "gpio_cfg.h"
/* ---------- 工作模式选择（1=使用 0=不使用） ---------- */
#define ADC_MODE_SINGLE       0   // 单通道轮询模式
#define ADC_MODE_DMA          0   // 多通道 DMA 模式
#define ADC_MODE_INJECTED     0   // 注入通道模式


//==========模式初始化======
void ADC_init();

void ADC_get();
#endif