#include "EXTI.h"
#include "EXTI_config.h"
#include "gd32f4xx_exti.h"
#include "gd32f4xx_syscfg.h"

/* ==================== 弱函数宏（GCC/AC6 标准写法） ==================== */
#define BSP_WEAK __attribute__((weak))

/* ==================== 自动推导宏 ==================== */
#define _EXTI_PIN(n)         GPIO_PIN_##n
#define EXTI_PIN(n)          _EXTI_PIN(n)

#define _EXTI_SOURCE_PIN(n)  EXTI_SOURCE_PIN##n
#define EXTI_SOURCE_PIN(n)   _EXTI_SOURCE_PIN(n)

#define _EXTI_LINE(n)        EXTI_##n
#define EXTI_LINE(n)         _EXTI_LINE(n)

#define _EXTI_IRQn(n)        EXTI##n##_IRQn
#define EXTI_IRQn(n)         _EXTI_IRQn(n)

/* ==================== 配置检查 ==================== */
#if (USE_EXTI_0  == 0 && USE_EXTI_1  == 0 && USE_EXTI_2  == 0 && \
     USE_EXTI_3  == 0 && USE_EXTI_4  == 0 && USE_EXTI_5  == 0 && \
     USE_EXTI_6  == 0 && USE_EXTI_7  == 0 && USE_EXTI_8  == 0 && \
     USE_EXTI_9  == 0 && USE_EXTI_10 == 0 && USE_EXTI_11 == 0 && \
     USE_EXTI_12 == 0 && USE_EXTI_13 == 0 && USE_EXTI_14 == 0 && \
     USE_EXTI_15 == 0)
    #error "BSP EXTI: 请至少启用一个 EXTI（将某个 USE_EXTI_x 设为 1）"
#endif

/* ==================== 内部函数 ==================== */
static void bsp_exti_line_init(rcu_periph_enum rcu,
                               uint32_t port,
                               uint32_t pin,
                               uint32_t pupd,
                               uint8_t source_port,
                               uint8_t source_pin,
                               exti_line_enum line,
                               exti_trig_type_enum trig_type,
                               uint8_t irqn,
                               uint8_t pre_prio,
                               uint8_t sub_prio)
{
    rcu_periph_clock_enable(rcu);
    gpio_mode_set(port, GPIO_MODE_INPUT, pupd, pin);

    rcu_periph_clock_enable(RCU_SYSCFG);
    syscfg_exti_line_config(source_port, source_pin);

    exti_init(line, EXTI_INTERRUPT, trig_type);
    exti_interrupt_flag_clear(line);
    exti_interrupt_enable(line);

    nvic_irq_enable(irqn, pre_prio, sub_prio);
}

static void bsp_exti_irq_handle(exti_line_enum line, void (*callback)(void))
{
    if (SET == exti_interrupt_flag_get(line)) {
        exti_interrupt_flag_clear(line);
        if (callback != NULL) {
            callback();
        }
    }
}

/* ==================== 初始化入口 ==================== */
void EXTI_Init(void)
{
#if (USE_EXTI_0 == 1)
    bsp_exti_line_init(EXTI0_RCU, EXTI0_PORT, EXTI_PIN(0), EXTI0_PUPD,
                       EXTI0_SOURCE_PORT, EXTI_SOURCE_PIN(0), EXTI_LINE(0),
                       EXTI0_TRIG_TYPE, EXTI_IRQn(0),
                       EXTI0_PRE_PRIORITY, EXTI0_SUB_PRIORITY);
#endif

#if (USE_EXTI_1 == 1)
    bsp_exti_line_init(EXTI1_RCU, EXTI1_PORT, EXTI_PIN(1), EXTI1_PUPD,
                       EXTI1_SOURCE_PORT, EXTI_SOURCE_PIN(1), EXTI_LINE(1),
                       EXTI1_TRIG_TYPE, EXTI_IRQn(1),
                       EXTI1_PRE_PRIORITY, EXTI1_SUB_PRIORITY);
#endif

#if (USE_EXTI_2 == 1)
    bsp_exti_line_init(EXTI2_RCU, EXTI2_PORT, EXTI_PIN(2), EXTI2_PUPD,
                       EXTI2_SOURCE_PORT, EXTI_SOURCE_PIN(2), EXTI_LINE(2),
                       EXTI2_TRIG_TYPE, EXTI_IRQn(2),
                       EXTI2_PRE_PRIORITY, EXTI2_SUB_PRIORITY);
#endif

#if (USE_EXTI_3 == 1)
    bsp_exti_line_init(EXTI3_RCU, EXTI3_PORT, EXTI_PIN(3), EXTI3_PUPD,
                       EXTI3_SOURCE_PORT, EXTI_SOURCE_PIN(3), EXTI_LINE(3),
                       EXTI3_TRIG_TYPE, EXTI_IRQn(3),
                       EXTI3_PRE_PRIORITY, EXTI3_SUB_PRIORITY);
#endif

#if (USE_EXTI_4 == 1)
    bsp_exti_line_init(EXTI4_RCU, EXTI4_PORT, EXTI_PIN(4), EXTI4_PUPD,
                       EXTI4_SOURCE_PORT, EXTI_SOURCE_PIN(4), EXTI_LINE(4),
                       EXTI4_TRIG_TYPE, EXTI_IRQn(4),
                       EXTI4_PRE_PRIORITY, EXTI4_SUB_PRIORITY);
#endif

#if (USE_EXTI_5 == 1)
    bsp_exti_line_init(EXTI5_RCU, EXTI5_PORT, EXTI_PIN(5), EXTI5_PUPD,
                       EXTI5_SOURCE_PORT, EXTI_SOURCE_PIN(5), EXTI_LINE(5),
                       EXTI5_TRIG_TYPE, EXTI_IRQn(5),
                       EXTI5_PRE_PRIORITY, EXTI5_SUB_PRIORITY);
#endif

#if (USE_EXTI_6 == 1)
    bsp_exti_line_init(EXTI6_RCU, EXTI6_PORT, EXTI_PIN(6), EXTI6_PUPD,
                       EXTI6_SOURCE_PORT, EXTI_SOURCE_PIN(6), EXTI_LINE(6),
                       EXTI6_TRIG_TYPE, EXTI_IRQn(6),
                       EXTI6_PRE_PRIORITY, EXTI6_SUB_PRIORITY);
#endif

#if (USE_EXTI_7 == 1)
    bsp_exti_line_init(EXTI7_RCU, EXTI7_PORT, EXTI_PIN(7), EXTI7_PUPD,
                       EXTI7_SOURCE_PORT, EXTI_SOURCE_PIN(7), EXTI_LINE(7),
                       EXTI7_TRIG_TYPE, EXTI_IRQn(7),
                       EXTI7_PRE_PRIORITY, EXTI7_SUB_PRIORITY);
#endif

#if (USE_EXTI_8 == 1)
    bsp_exti_line_init(EXTI8_RCU, EXTI8_PORT, EXTI_PIN(8), EXTI8_PUPD,
                       EXTI8_SOURCE_PORT, EXTI_SOURCE_PIN(8), EXTI_LINE(8),
                       EXTI8_TRIG_TYPE, EXTI_IRQn(8),
                       EXTI8_PRE_PRIORITY, EXTI8_SUB_PRIORITY);
#endif

#if (USE_EXTI_9 == 1)
    bsp_exti_line_init(EXTI9_RCU, EXTI9_PORT, EXTI_PIN(9), EXTI9_PUPD,
                       EXTI9_SOURCE_PORT, EXTI_SOURCE_PIN(9), EXTI_LINE(9),
                       EXTI9_TRIG_TYPE, EXTI_IRQn(9),
                       EXTI9_PRE_PRIORITY, EXTI9_SUB_PRIORITY);
#endif

#if (USE_EXTI_10 == 1)
    bsp_exti_line_init(EXTI10_RCU, EXTI10_PORT, EXTI_PIN(10), EXTI10_PUPD,
                       EXTI10_SOURCE_PORT, EXTI_SOURCE_PIN(10), EXTI_LINE(10),
                       EXTI10_TRIG_TYPE, EXTI_IRQn(10),
                       EXTI10_PRE_PRIORITY, EXTI10_SUB_PRIORITY);
#endif

#if (USE_EXTI_11 == 1)
    bsp_exti_line_init(EXTI11_RCU, EXTI11_PORT, EXTI_PIN(11), EXTI11_PUPD,
                       EXTI11_SOURCE_PORT, EXTI_SOURCE_PIN(11), EXTI_LINE(11),
                       EXTI11_TRIG_TYPE, EXTI_IRQn(11),
                       EXTI11_PRE_PRIORITY, EXTI11_SUB_PRIORITY);
#endif

#if (USE_EXTI_12 == 1)
    bsp_exti_line_init(EXTI12_RCU, EXTI12_PORT, EXTI_PIN(12), EXTI12_PUPD,
                       EXTI12_SOURCE_PORT, EXTI_SOURCE_PIN(12), EXTI_LINE(12),
                       EXTI12_TRIG_TYPE, EXTI_IRQn(12),
                       EXTI12_PRE_PRIORITY, EXTI12_SUB_PRIORITY);
#endif

#if (USE_EXTI_13 == 1)
    bsp_exti_line_init(EXTI13_RCU, EXTI13_PORT, EXTI_PIN(13), EXTI13_PUPD,
                       EXTI13_SOURCE_PORT, EXTI_SOURCE_PIN(13), EXTI_LINE(13),
                       EXTI13_TRIG_TYPE, EXTI_IRQn(13),
                       EXTI13_PRE_PRIORITY, EXTI13_SUB_PRIORITY);
#endif

#if (USE_EXTI_14 == 1)
    bsp_exti_line_init(EXTI14_RCU, EXTI14_PORT, EXTI_PIN(14), EXTI14_PUPD,
                       EXTI14_SOURCE_PORT, EXTI_SOURCE_PIN(14), EXTI_LINE(14),
                       EXTI14_TRIG_TYPE, EXTI_IRQn(14),
                       EXTI14_PRE_PRIORITY, EXTI14_SUB_PRIORITY);
#endif

#if (USE_EXTI_15 == 1)
    bsp_exti_line_init(EXTI15_RCU, EXTI15_PORT, EXTI_PIN(15), EXTI15_PUPD,
                       EXTI15_SOURCE_PORT, EXTI_SOURCE_PIN(15), EXTI_LINE(15),
                       EXTI15_TRIG_TYPE, EXTI_IRQn(15),
                       EXTI15_PRE_PRIORITY, EXTI15_SUB_PRIORITY);
#endif
}

/* ==================== IRQHandler ==================== */

#if (USE_EXTI_0 == 1)
void EXTI0_IRQHandler(void)
{
    bsp_exti_irq_handle(EXTI_LINE(0), EXTI0_on_trig);
}
#endif

#if (USE_EXTI_1 == 1)
void EXTI1_IRQHandler(void)
{
    bsp_exti_irq_handle(EXTI_LINE(1), EXTI1_on_trig);
}
#endif

#if (USE_EXTI_2 == 1)
void EXTI2_IRQHandler(void)
{
    bsp_exti_irq_handle(EXTI_LINE(2), EXTI2_on_trig);
}
#endif

#if (USE_EXTI_3 == 1)
void EXTI3_IRQHandler(void)
{
    bsp_exti_irq_handle(EXTI_LINE(3), EXTI3_on_trig);
}
#endif

#if (USE_EXTI_4 == 1)
void EXTI4_IRQHandler(void)
{
    bsp_exti_irq_handle(EXTI_LINE(4), EXTI4_on_trig);
}
#endif

#if (USE_EXTI_5 == 1 || USE_EXTI_6 == 1 || USE_EXTI_7 == 1 || USE_EXTI_8 == 1 || USE_EXTI_9 == 1)
void EXTI5_9_IRQHandler(void)
{
#if (USE_EXTI_5 == 1)
    bsp_exti_irq_handle(EXTI_LINE(5), EXTI5_on_trig);
#endif
#if (USE_EXTI_6 == 1)
    bsp_exti_irq_handle(EXTI_LINE(6), EXTI6_on_trig);
#endif
#if (USE_EXTI_7 == 1)
    bsp_exti_irq_handle(EXTI_LINE(7), EXTI7_on_trig);
#endif
#if (USE_EXTI_8 == 1)
    bsp_exti_irq_handle(EXTI_LINE(8), EXTI8_on_trig);
#endif
#if (USE_EXTI_9 == 1)
    bsp_exti_irq_handle(EXTI_LINE(9), EXTI9_on_trig);
#endif
}
#endif

#if (USE_EXTI_10 == 1 || USE_EXTI_11 == 1 || USE_EXTI_12 == 1 || USE_EXTI_13 == 1 || USE_EXTI_14 == 1 || USE_EXTI_15 == 1)
void EXTI10_15_IRQHandler(void)
{
#if (USE_EXTI_10 == 1)
    bsp_exti_irq_handle(EXTI_LINE(10), EXTI10_on_trig);
#endif
#if (USE_EXTI_11 == 1)
    bsp_exti_irq_handle(EXTI_LINE(11), EXTI11_on_trig);
#endif
#if (USE_EXTI_12 == 1)
    bsp_exti_irq_handle(EXTI_LINE(12), EXTI12_on_trig);
#endif
#if (USE_EXTI_13 == 1)
    bsp_exti_irq_handle(EXTI_LINE(13), EXTI13_on_trig);
#endif
#if (USE_EXTI_14 == 1)
    bsp_exti_irq_handle(EXTI_LINE(14), EXTI14_on_trig);
#endif
#if (USE_EXTI_15 == 1)
    bsp_exti_irq_handle(EXTI_LINE(15), EXTI15_on_trig);
#endif
}
#endif

/* ==================== 默认空实现（弱函数） ==================== */
BSP_WEAK void EXTI0_on_trig(void)  {}
BSP_WEAK void EXTI1_on_trig(void)  {}
BSP_WEAK void EXTI2_on_trig(void)  {}
BSP_WEAK void EXTI3_on_trig(void)  {}
BSP_WEAK void EXTI4_on_trig(void)  {}
BSP_WEAK void EXTI5_on_trig(void)  {}
BSP_WEAK void EXTI6_on_trig(void)  {}
BSP_WEAK void EXTI7_on_trig(void)  {}
BSP_WEAK void EXTI8_on_trig(void)  {}
BSP_WEAK void EXTI9_on_trig(void)  {}
BSP_WEAK void EXTI10_on_trig(void) {}
BSP_WEAK void EXTI11_on_trig(void) {}
BSP_WEAK void EXTI12_on_trig(void) {}
BSP_WEAK void EXTI13_on_trig(void) {}
BSP_WEAK void EXTI14_on_trig(void) {}
BSP_WEAK void EXTI15_on_trig(void) {}