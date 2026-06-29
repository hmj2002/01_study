#ifndef __TIMER_H__
#define __TIMER_H__

#include "gd32f4xx.h"
#include "gpio_cfg.h"
#include "TIMER_config.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f4xx_timer.h"
// 初始化所有在 TIMER_config.h 中启用的定时器
void TIMER_init(void);

/**
 * @brief 启动定时器
 *
 * @param timer_periph TIMERx
 */
void TIMER_start(uint32_t timer_periph);

/**
 * @brief 停止定时器
 *
 * @param timer_periph TIMERx
 */
void TIMER_stop(uint32_t timer_periph);

/**
 * @brief 更新指定 Timer 的 channel 占空比
 *
 * @param timer_periph TIMERx(x=0,1,2,3,4,5,6,7,8,9,10,11,12,13)
 * @param channel      TIMER_CH_0, TIMER_CH_1, TIMER_CH_2, TIMER_CH_3
 * @param t_period     周期计数值，例如 TM3_PERIOD
 * @param duty         占空比 [0.0, 100.0]
 */
void TIMER_channel_duty_update(uint32_t timer_periph,
                               uint16_t channel,
                               uint32_t t_period,
                               float duty);

/**
 * @brief 直接更新比较值
 *
 * @param timer_periph TIMERx
 * @param channel      TIMER_CH_x
 * @param t_period     周期计数值
 * @param pulse        比较值 [0, t_period]
 */
void TIMER_channel_pulse_update(uint32_t timer_periph,
                                uint16_t channel,
                                uint32_t t_period,
                                uint32_t pulse);

#endif
