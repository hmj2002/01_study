#ifndef __EXTI_CONFIG_H
#define __EXTI_CONFIG_H

#include "gd32f4xx.h"

/* 开关 */
#define USE_EXTI_0      0
#define USE_EXTI_1      0
#define USE_EXTI_2      0
#define USE_EXTI_3      0
#define USE_EXTI_4      0
#define USE_EXTI_5      0
#define USE_EXTI_6      0
#define USE_EXTI_7      0
#define USE_EXTI_8      0
#define USE_EXTI_9      0
#define USE_EXTI_10     0
#define USE_EXTI_11     1
#define USE_EXTI_12     0
#define USE_EXTI_13     0
#define USE_EXTI_14     0
#define USE_EXTI_15     1

/* ============================================================
 * 用户只需配置：RCU、PORT、SOURCE_PORT、PUPD、TRIG_TYPE、优先级
 * 
 * 以下自动推导（无需定义）：
 *   PIN          → GPIO_PIN_x
 *   SOURCE_PIN   → EXTI_SOURCE_PINx
 *   LINE         → EXTI_x
 *   IRQn         → EXTIx_IRQn
 * ============================================================ */

/* ---------- EXTI0 ---------- */
#define EXTI0_RCU           RCU_GPIOC
#define EXTI0_PORT          GPIOC
#define EXTI0_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI0_PUPD          GPIO_PUPD_PULLUP
#define EXTI0_TRIG_TYPE     EXTI_TRIG_BOTH
#define EXTI0_PRE_PRIORITY  2
#define EXTI0_SUB_PRIORITY  2

/* ---------- EXTI1 ---------- */
#define EXTI1_RCU           RCU_GPIOC
#define EXTI1_PORT          GPIOC
#define EXTI1_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI1_PUPD          GPIO_PUPD_PULLUP
#define EXTI1_TRIG_TYPE     EXTI_TRIG_FALLING
#define EXTI1_PRE_PRIORITY  2
#define EXTI1_SUB_PRIORITY  2

/* ---------- EXTI2 ---------- */
#define EXTI2_RCU           RCU_GPIOC
#define EXTI2_PORT          GPIOC
#define EXTI2_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI2_PUPD          GPIO_PUPD_PULLUP
#define EXTI2_TRIG_TYPE     EXTI_TRIG_BOTH
#define EXTI2_PRE_PRIORITY  2
#define EXTI2_SUB_PRIORITY  2

/* ---------- EXTI3 ---------- */
#define EXTI3_RCU           RCU_GPIOA
#define EXTI3_PORT          GPIOA
#define EXTI3_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI3_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI3_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI3_PRE_PRIORITY  2
#define EXTI3_SUB_PRIORITY  2

/* ---------- EXTI4 ---------- */
#define EXTI4_RCU           RCU_GPIOA
#define EXTI4_PORT          GPIOA
#define EXTI4_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI4_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI4_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI4_PRE_PRIORITY  2
#define EXTI4_SUB_PRIORITY  2

/* ---------- EXTI5 ---------- */
#define EXTI5_RCU           RCU_GPIOA
#define EXTI5_PORT          GPIOA
#define EXTI5_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI5_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI5_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI5_PRE_PRIORITY  2
#define EXTI5_SUB_PRIORITY  2

/* ---------- EXTI6 ---------- */
#define EXTI6_RCU           RCU_GPIOA
#define EXTI6_PORT          GPIOA
#define EXTI6_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI6_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI6_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI6_PRE_PRIORITY  2
#define EXTI6_SUB_PRIORITY  2

/* ---------- EXTI7 ---------- */
#define EXTI7_RCU           RCU_GPIOA
#define EXTI7_PORT          GPIOA
#define EXTI7_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI7_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI7_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI7_PRE_PRIORITY  2
#define EXTI7_SUB_PRIORITY  2

/* ---------- EXTI8 ---------- */
#define EXTI8_RCU           RCU_GPIOA
#define EXTI8_PORT          GPIOA
#define EXTI8_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI8_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI8_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI8_PRE_PRIORITY  2
#define EXTI8_SUB_PRIORITY  2

/* ---------- EXTI9 ---------- */
#define EXTI9_RCU           RCU_GPIOA
#define EXTI9_PORT          GPIOA
#define EXTI9_SOURCE_PORT   EXTI_SOURCE_GPIOA
#define EXTI9_PUPD          GPIO_PUPD_PULLDOWN
#define EXTI9_TRIG_TYPE     EXTI_TRIG_RISING
#define EXTI9_PRE_PRIORITY  2
#define EXTI9_SUB_PRIORITY  2

/* ---------- EXTI10 ---------- */
#define EXTI10_RCU          RCU_GPIOA
#define EXTI10_PORT         GPIOA
#define EXTI10_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define EXTI10_PUPD         GPIO_PUPD_PULLDOWN
#define EXTI10_TRIG_TYPE    EXTI_TRIG_RISING
#define EXTI10_PRE_PRIORITY 2
#define EXTI10_SUB_PRIORITY 2

/* ---------- EXTI11 ---------- */
#define EXTI11_RCU          RCU_GPIOD
#define EXTI11_PORT         GPIOD
#define EXTI11_SOURCE_PORT  EXTI_SOURCE_GPIOD
#define EXTI11_PUPD         GPIO_PUPD_NONE
#define EXTI11_TRIG_TYPE    EXTI_TRIG_BOTH
#define EXTI11_PRE_PRIORITY 2
#define EXTI11_SUB_PRIORITY 2

/* ---------- EXTI12 ---------- */
#define EXTI12_RCU          RCU_GPIOA
#define EXTI12_PORT         GPIOA
#define EXTI12_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define EXTI12_PUPD         GPIO_PUPD_PULLDOWN
#define EXTI12_TRIG_TYPE    EXTI_TRIG_RISING
#define EXTI12_PRE_PRIORITY 2
#define EXTI12_SUB_PRIORITY 2

/* ---------- EXTI13 ---------- */
#define EXTI13_RCU          RCU_GPIOA
#define EXTI13_PORT         GPIOA
#define EXTI13_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define EXTI13_PUPD         GPIO_PUPD_PULLDOWN
#define EXTI13_TRIG_TYPE    EXTI_TRIG_RISING
#define EXTI13_PRE_PRIORITY 2
#define EXTI13_SUB_PRIORITY 2

/* ---------- EXTI14 ---------- */
#define EXTI14_RCU          RCU_GPIOA
#define EXTI14_PORT         GPIOA
#define EXTI14_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define EXTI14_PUPD         GPIO_PUPD_PULLDOWN
#define EXTI14_TRIG_TYPE    EXTI_TRIG_RISING
#define EXTI14_PRE_PRIORITY 2
#define EXTI14_SUB_PRIORITY 2

/* ---------- EXTI15 ---------- */
#define EXTI15_RCU          RCU_GPIOD
#define EXTI15_PORT         GPIOD
#define EXTI15_SOURCE_PORT  EXTI_SOURCE_GPIOD
#define EXTI15_PUPD         GPIO_PUPD_NONE    //===旋转按钮按键引脚
#define EXTI15_TRIG_TYPE    EXTI_TRIG_BOTH//EXTI_TRIG_FALLING：下降沿     EXTI_TRIG_RISING：上升沿    EXTI_TRIG_BOTH：都检测
#define EXTI15_PRE_PRIORITY 2
#define EXTI15_SUB_PRIORITY 2

#endif /* __EXTI_CONFIG_H */