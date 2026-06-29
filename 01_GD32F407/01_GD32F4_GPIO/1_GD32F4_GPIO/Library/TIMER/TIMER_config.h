#ifndef __TIMER_CONFIG_H__
#define __TIMER_CONFIG_H__

#include "gd32f4xx.h"

/*
 * ============================================================
 * 通用配置
 * ============================================================
 */

/*
 * GD32F407 的 TIMER 时钟和时钟树配置有关。
 * 如果你的时钟树中 TIMER 实际时钟就是 SystemCoreClock，可以保持默认。
 * 如果你后面改了 APB 分频，导致 TIMER 时钟不是 SystemCoreClock，
 * 这里需要改成真实 TIMER 时钟。
 */
#ifndef TIMER_CLK_HZ
#define TIMER_CLK_HZ        SystemCoreClock
#endif

/*
 * TIMER 计数周期计算
 *
 * PWM频率 = TIMER_CLK_HZ / prescaler / period
 *
 * 注意：
 * prescaler 传入的是实际分频值，比如 10 表示 10 分频；
 * 底层会自动写入 prescaler - 1。
 */
#define TIMER_PERIOD_CALC(prescaler, freq)    ((uint32_t)(TIMER_CLK_HZ / (prescaler) / (freq)))

/*
 * TIMER 时钟预分频配置
 * 你原来使用的是 RCU_TIMER_PSC_MUL4，这里保留。
 */
#define TIMER_RCU_PSC       RCU_TIMER_PSC_MUL4

/*
 * PWM模式：
 * TIMER_OC_MODE_PWM0：正常 PWM
 * TIMER_OC_MODE_PWM1：反向 PWM
 */
#define TIMER_PWM_MODE      TIMER_OC_MODE_PWM0

/*
 * 高级定时器互补输出死区时间 [0, 255]
 * 如果只是点 LED 或控制普通小负载，0 即可。
 * 如果是驱动半桥/全桥，需要根据 MOS 管参数配置。
 */
#define TIMER_ADV_DEADTIME  0

/*
 * 是否启用高级定时器 Break 输入
 *
 * 0：关闭 Break 输入，推荐默认关闭，否则 Break 引脚电平异常可能导致无输出。
 * 1：启用 Break 输入。
 */
#define TIMER_ADV_BREAK_ENABLE  0


/*
 * ============================================================
 * 是否启用各 TIMER
 * ============================================================
 */

#define USE_TIMER_0     0       // 高级定时器，支持互补输出
#define USE_TIMER_1     0
#define USE_TIMER_2     0
#define USE_TIMER_3     0       // LED5 ~ LED8
#define USE_TIMER_4     0
#define USE_TIMER_5     0       // 基本定时器，无 PWM 输出
#define USE_TIMER_6     0       // 基本定时器，无 PWM 输出
#define USE_TIMER_7     0       // 高级定时器，支持互补输出
#define USE_TIMER_8     0
#define USE_TIMER_9     0       // 震动马达
#define USE_TIMER_10    0
#define USE_TIMER_11    0
#define USE_TIMER_12    0
#define USE_TIMER_13    0


/*
 * ============================================================
 * TIMER0 高级定时器
 * ============================================================
 */

#if USE_TIMER_0

#define TM0_PRESCALER   10
#define TM0_FREQ        1000
#define TM0_PERIOD      TIMER_PERIOD_CALC(TM0_PRESCALER, TM0_FREQ)

/*
 * 高级定时器命名：
 *
 * OP：普通输出，例如 TIMER0_CH0
 * ON：互补输出，例如 TIMER0_CH0N
 *
 * 你现在的板子：
 * TIMER0_CH0N PE8
 * TIMER0_CH0  PE9
 */

// TIMER0_CH0N PE8
#define TM0_CH0_ON      RCU_GPIOE, GPIOE, GPIO_PIN_8,  GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH0  PE9
#define TM0_CH0_OP      RCU_GPIOE, GPIOE, GPIO_PIN_9,  GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH1N PE10
// #define TM0_CH1_ON   RCU_GPIOE, GPIOE, GPIO_PIN_10, GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH1  PE11
// #define TM0_CH1_OP   RCU_GPIOE, GPIOE, GPIO_PIN_11, GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH2N PE12
// #define TM0_CH2_ON   RCU_GPIOE, GPIOE, GPIO_PIN_12, GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH2  PE13
// #define TM0_CH2_OP   RCU_GPIOE, GPIOE, GPIO_PIN_13, GPIO_OTYPE_PP, GPIO_AF_1

// TIMER0_CH3 PE14，一般没有互补输出
// #define TM0_CH3_OP   RCU_GPIOE, GPIOE, GPIO_PIN_14, GPIO_OTYPE_PP, GPIO_AF_1

#endif


/*
 * ============================================================
 * TIMER1
 * ============================================================
 */

#if USE_TIMER_1

#define TM1_PRESCALER   10
#define TM1_FREQ        1000
#define TM1_PERIOD      TIMER_PERIOD_CALC(TM1_PRESCALER, TM1_FREQ)

/*
 * 注意：
 * 你当前 TIMER9_CH0 也用了 PB8。
 * 如果启用 TIMER1_CH0 PB8，会和 TIMER9_CH0 PB8 冲突。
 */
// #define TM1_CH0      RCU_GPIOB, GPIOB, GPIO_PIN_8, GPIO_OTYPE_PP, GPIO_AF_1
// #define TM1_CH1      RCU_GPIOB, GPIOB, GPIO_PIN_9, GPIO_OTYPE_PP, GPIO_AF_1
// #define TM1_CH2      RCU_GPIOB, GPIOB, GPIO_PIN_10, GPIO_OTYPE_PP, GPIO_AF_1
// #define TM1_CH3      RCU_GPIOB, GPIOB, GPIO_PIN_11, GPIO_OTYPE_PP, GPIO_AF_1

#endif


/*
 * ============================================================
 * TIMER2
 * ============================================================
 */

#if USE_TIMER_2

#define TM2_PRESCALER   10
#define TM2_FREQ        1000
#define TM2_PERIOD      TIMER_PERIOD_CALC(TM2_PRESCALER, TM2_FREQ)

// 根据手册选择实际引脚
// #define TM2_CH0      RCU_GPIOA, GPIOA, GPIO_PIN_6, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM2_CH1      RCU_GPIOA, GPIOA, GPIO_PIN_7, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM2_CH2      RCU_GPIOB, GPIOB, GPIO_PIN_0, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM2_CH3      RCU_GPIOB, GPIOB, GPIO_PIN_1, GPIO_OTYPE_PP, GPIO_AF_2

#endif


/*
 * ============================================================
 * TIMER3
 * ============================================================
 */

#if USE_TIMER_3

#define TM3_PRESCALER   10
#define TM3_FREQ        1000
#define TM3_PERIOD      TIMER_PERIOD_CALC(TM3_PRESCALER, TM3_FREQ)

// TIMER3_CH0 PD12
#define TM3_CH0         RCU_GPIOD, GPIOD, GPIO_PIN_12, GPIO_OTYPE_PP, GPIO_AF_2

// TIMER3_CH1 PD13
#define TM3_CH1         RCU_GPIOD, GPIOD, GPIO_PIN_13, GPIO_OTYPE_PP, GPIO_AF_2

// TIMER3_CH2 PD14
#define TM3_CH2         RCU_GPIOD, GPIOD, GPIO_PIN_14, GPIO_OTYPE_PP, GPIO_AF_2

// TIMER3_CH3 PD15
#define TM3_CH3         RCU_GPIOD, GPIOD, GPIO_PIN_15, GPIO_OTYPE_PP, GPIO_AF_2

#endif


/*
 * ============================================================
 * TIMER4
 * ============================================================
 */

#if USE_TIMER_4

#define TM4_PRESCALER   10
#define TM4_FREQ        1000
#define TM4_PERIOD      TIMER_PERIOD_CALC(TM4_PRESCALER, TM4_FREQ)

// #define TM4_CH0      RCU_GPIOA, GPIOA, GPIO_PIN_0, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM4_CH1      RCU_GPIOA, GPIOA, GPIO_PIN_1, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM4_CH2      RCU_GPIOA, GPIOA, GPIO_PIN_2, GPIO_OTYPE_PP, GPIO_AF_2
// #define TM4_CH3      RCU_GPIOA, GPIOA, GPIO_PIN_3, GPIO_OTYPE_PP, GPIO_AF_2

#endif


/*
 * ============================================================
 * TIMER5 / TIMER6 基本定时器
 * 一般只用于定时中断，不用于 PWM。
 * 如果后续要封装定时中断，可以继续扩展。
 * ============================================================
 */

#if USE_TIMER_5

#define TM5_PRESCALER   10000
#define TM5_FREQ        100
#define TM5_PERIOD      TIMER_PERIOD_CALC(TM5_PRESCALER, TM5_FREQ)

#endif

#if USE_TIMER_6

#define TM6_PRESCALER   10000
#define TM6_FREQ        100
#define TM6_PERIOD      TIMER_PERIOD_CALC(TM6_PRESCALER, TM6_FREQ)

#endif


/*
 * ============================================================
 * TIMER7 高级定时器
 * ============================================================
 */

#if USE_TIMER_7

#define TM7_PRESCALER   10
#define TM7_FREQ        1000
#define TM7_PERIOD      TIMER_PERIOD_CALC(TM7_PRESCALER, TM7_FREQ)

// 示例，具体引脚请按手册修改
// #define TM7_CH0_ON   RCU_GPIOA, GPIOA, GPIO_PIN_7, GPIO_OTYPE_PP, GPIO_AF_3
// #define TM7_CH0_OP   RCU_GPIOC, GPIOC, GPIO_PIN_6, GPIO_OTYPE_PP, GPIO_AF_3

#endif


/*
 * ============================================================
 * TIMER8
 * ============================================================
 */

#if USE_TIMER_8

#define TM8_PRESCALER   10
#define TM8_FREQ        1000
#define TM8_PERIOD      TIMER_PERIOD_CALC(TM8_PRESCALER, TM8_FREQ)

// #define TM8_CH0      RCU_GPIOE, GPIOE, GPIO_PIN_5, GPIO_OTYPE_PP, GPIO_AF_3
// #define TM8_CH1      RCU_GPIOE, GPIOE, GPIO_PIN_6, GPIO_OTYPE_PP, GPIO_AF_3

#endif


/*
 * ============================================================
 * TIMER9
 * ============================================================
 */

#if USE_TIMER_9

#define TM9_PRESCALER   10
#define TM9_FREQ        1000
#define TM9_PERIOD      TIMER_PERIOD_CALC(TM9_PRESCALER, TM9_FREQ)

// TIMER9_CH0 PB8
#define TM9_CH0         RCU_GPIOB, GPIOB, GPIO_PIN_8, GPIO_OTYPE_PP, GPIO_AF_3

#endif


/*
 * ============================================================
 * TIMER10
 * ============================================================
 */

#if USE_TIMER_10

#define TM10_PRESCALER  10
#define TM10_FREQ       1000
#define TM10_PERIOD     TIMER_PERIOD_CALC(TM10_PRESCALER, TM10_FREQ)

// #define TM10_CH0     RCU_GPIOB, GPIOB, GPIO_PIN_9, GPIO_OTYPE_PP, GPIO_AF_3

#endif


/*
 * ============================================================
 * TIMER11
 * ============================================================
 */

#if USE_TIMER_11

#define TM11_PRESCALER  10
#define TM11_FREQ       1000
#define TM11_PERIOD     TIMER_PERIOD_CALC(TM11_PRESCALER, TM11_FREQ)

// #define TM11_CH0     RCU_GPIOB, GPIOB, GPIO_PIN_14, GPIO_OTYPE_PP, GPIO_AF_9
// #define TM11_CH1     RCU_GPIOB, GPIOB, GPIO_PIN_15, GPIO_OTYPE_PP, GPIO_AF_9

#endif


/*
 * ============================================================
 * TIMER12
 * ============================================================
 */

#if USE_TIMER_12

#define TM12_PRESCALER  10
#define TM12_FREQ       1000
#define TM12_PERIOD     TIMER_PERIOD_CALC(TM12_PRESCALER, TM12_FREQ)

// #define TM12_CH0     RCU_GPIOA, GPIOA, GPIO_PIN_6, GPIO_OTYPE_PP, GPIO_AF_9

#endif


/*
 * ============================================================
 * TIMER13
 * ============================================================
 */

#if USE_TIMER_13

#define TM13_PRESCALER  10
#define TM13_FREQ       1000
#define TM13_PERIOD     TIMER_PERIOD_CALC(TM13_PRESCALER, TM13_FREQ)

// #define TM13_CH0     RCU_GPIOA, GPIOA, GPIO_PIN_7, GPIO_OTYPE_PP, GPIO_AF_9

#endif

#endif
