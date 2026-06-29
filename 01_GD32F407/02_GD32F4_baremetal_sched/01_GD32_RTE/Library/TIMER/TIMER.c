#include "TIMER.h"


#define TIMER_PWM_OUT_P        1       // 普通输出
#define TIMER_PWM_OUT_N        2       // 互补输出
#define TIMER_PWM_OUT_BOTH     3       // 普通 + 互补输出


#define TIMER_GPIO_PWM_P_INIT(GPIO_DEF, TIMERX, CHX)       \
    do {                                                   \
        GPIO_output_af(GPIO_DEF);                          \
        timer_channel_config(TIMERX, CHX, TIMER_PWM_OUT_P);\
    } while (0)

#define TIMER_GPIO_PWM_N_INIT(GPIO_DEF, TIMERX, CHX)       \
    do {                                                   \
        GPIO_output_af(GPIO_DEF);                          \
        timer_channel_config(TIMERX, CHX, TIMER_PWM_OUT_N);\
    } while (0)

#define TIMER_GPIO_PWM_BOTH_INIT(GPIO_N_DEF, GPIO_P_DEF, TIMERX, CHX)      \
    do {                                                                    \
        GPIO_output_af(GPIO_N_DEF);                                         \
        GPIO_output_af(GPIO_P_DEF);                                         \
        timer_channel_config(TIMERX, CHX, TIMER_PWM_OUT_BOTH);              \
    } while (0)


/**
 * @brief Timer 基础计数配置
 *
 * @param rcu           TIMER 的 RCU
 * @param timer_periph  TIMERx
 * @param t_prescaler   分频值，例如 10 表示 10 分频
 * @param t_period      周期计数值
 */
static void timer_init_config(rcu_periph_enum rcu,
                              uint32_t timer_periph,
                              uint16_t t_prescaler,
                              uint32_t t_period) {
    timer_parameter_struct initpara;

    if (t_prescaler == 0) {
        t_prescaler = 1;
    }

    if (t_period == 0) {
        t_period = 1;
    }

    // 使能 TIMER 时钟
    rcu_periph_clock_enable(rcu);

    // 复位 TIMER 外设
    timer_deinit(timer_periph);

    // 初始化默认参数
    timer_struct_para_init(&initpara);

    // 计数周期
    initpara.period = t_period - 1;

    // 分频
    initpara.prescaler = t_prescaler - 1;

    // 初始化 TIMER
    timer_init(timer_periph, &initpara);

    // 自动重装载影子寄存器，使修改更平滑
    timer_auto_reload_shadow_enable(timer_periph);
}


/**
 * @brief PWM 输出通道配置
 *
 * @param timer_periph  TIMERx
 * @param channel       TIMER_CH_0, TIMER_CH_1, TIMER_CH_2, TIMER_CH_3
 * @param output_type   TIMER_PWM_OUT_P / TIMER_PWM_OUT_N / TIMER_PWM_OUT_BOTH
 */
static void timer_channel_config(uint32_t timer_periph,
                                 uint16_t channel,
                                 uint8_t output_type) {
    timer_oc_parameter_struct ocpara;

    // 初始化输出比较结构体
    timer_channel_output_struct_para_init(&ocpara);

    if (output_type == TIMER_PWM_OUT_P) {
        // 普通输出
        ocpara.outputstate = TIMER_CCX_ENABLE;
    } else if (output_type == TIMER_PWM_OUT_N) {
        // 互补输出
        ocpara.outputnstate = TIMER_CCXN_ENABLE;
    } else {
        // 普通 + 互补输出
        ocpara.outputstate = TIMER_CCX_ENABLE;
        ocpara.outputnstate = TIMER_CCXN_ENABLE;
    }

    // 配置输出比较通道
    timer_channel_output_config(timer_periph, channel, &ocpara);

    // 初始占空比 0%
    timer_channel_output_pulse_value_config(timer_periph, channel, 0);

    // 配置 PWM 模式
    timer_channel_output_mode_config(timer_periph, channel, TIMER_PWM_MODE);

    // 使能比较值影子寄存器
    timer_channel_output_shadow_config(timer_periph, channel, TIMER_OC_SHADOW_ENABLE);
}


/**
 * @brief 高级定时器输出配置
 *
 * TIMER0 / TIMER7 属于高级定时器。
 * 高级定时器要有输出，需要使能主输出。
 */
static void timer_advanced_output_config(uint32_t timer_periph) {
    timer_break_parameter_struct breakpara;

    timer_break_struct_para_init(&breakpara);

    breakpara.breakpolarity = TIMER_BREAK_POLARITY_HIGH;
    breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
    breakpara.deadtime = TIMER_ADV_DEADTIME;

#if TIMER_ADV_BREAK_ENABLE
    breakpara.breakstate = TIMER_BREAK_ENABLE;
#else
    breakpara.breakstate = TIMER_BREAK_DISABLE;
#endif

    timer_break_config(timer_periph, &breakpara);

    // 高级定时器主输出使能，非常重要
    timer_primary_output_config(timer_periph, ENABLE);
}


/**
 * @brief 初始化所有启用的 TIMER
 */
void TIMER_init(void) {
    /*
     * TIMER 时钟预分频
     * 你原来的代码使用 RCU_TIMER_PSC_MUL4，这里保留为配置项。
     */
    rcu_timer_clock_prescaler_config(TIMER_RCU_PSC);


#if USE_TIMER_0

    timer_init_config(RCU_TIMER0, TIMER0, TM0_PRESCALER, TM0_PERIOD);

#if defined(TM0_CH0_ON) && defined(TM0_CH0_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM0_CH0_ON, TM0_CH0_OP, TIMER0, TIMER_CH_0);
#elif defined(TM0_CH0_OP)
    TIMER_GPIO_PWM_P_INIT(TM0_CH0_OP, TIMER0, TIMER_CH_0);
#elif defined(TM0_CH0_ON)
    TIMER_GPIO_PWM_N_INIT(TM0_CH0_ON, TIMER0, TIMER_CH_0);
#endif

#if defined(TM0_CH1_ON) && defined(TM0_CH1_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM0_CH1_ON, TM0_CH1_OP, TIMER0, TIMER_CH_1);
#elif defined(TM0_CH1_OP)
    TIMER_GPIO_PWM_P_INIT(TM0_CH1_OP, TIMER0, TIMER_CH_1);
#elif defined(TM0_CH1_ON)
    TIMER_GPIO_PWM_N_INIT(TM0_CH1_ON, TIMER0, TIMER_CH_1);
#endif

#if defined(TM0_CH2_ON) && defined(TM0_CH2_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM0_CH2_ON, TM0_CH2_OP, TIMER0, TIMER_CH_2);
#elif defined(TM0_CH2_OP)
    TIMER_GPIO_PWM_P_INIT(TM0_CH2_OP, TIMER0, TIMER_CH_2);
#elif defined(TM0_CH2_ON)
    TIMER_GPIO_PWM_N_INIT(TM0_CH2_ON, TIMER0, TIMER_CH_2);
#endif

#if defined(TM0_CH3_ON) && defined(TM0_CH3_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM0_CH3_ON, TM0_CH3_OP, TIMER0, TIMER_CH_3);
#elif defined(TM0_CH3_OP)
    TIMER_GPIO_PWM_P_INIT(TM0_CH3_OP, TIMER0, TIMER_CH_3);
#elif defined(TM0_CH3_ON)
    TIMER_GPIO_PWM_N_INIT(TM0_CH3_ON, TIMER0, TIMER_CH_3);
#endif

    timer_advanced_output_config(TIMER0);
    timer_enable(TIMER0);

#endif


#if USE_TIMER_1

    timer_init_config(RCU_TIMER1, TIMER1, TM1_PRESCALER, TM1_PERIOD);

#if defined(TM1_CH0)
    TIMER_GPIO_PWM_P_INIT(TM1_CH0, TIMER1, TIMER_CH_0);
#endif

#if defined(TM1_CH1)
    TIMER_GPIO_PWM_P_INIT(TM1_CH1, TIMER1, TIMER_CH_1);
#endif

#if defined(TM1_CH2)
    TIMER_GPIO_PWM_P_INIT(TM1_CH2, TIMER1, TIMER_CH_2);
#endif

#if defined(TM1_CH3)
    TIMER_GPIO_PWM_P_INIT(TM1_CH3, TIMER1, TIMER_CH_3);
#endif

    timer_enable(TIMER1);

#endif


#if USE_TIMER_2

    timer_init_config(RCU_TIMER2, TIMER2, TM2_PRESCALER, TM2_PERIOD);

#if defined(TM2_CH0)
    TIMER_GPIO_PWM_P_INIT(TM2_CH0, TIMER2, TIMER_CH_0);
#endif

#if defined(TM2_CH1)
    TIMER_GPIO_PWM_P_INIT(TM2_CH1, TIMER2, TIMER_CH_1);
#endif

#if defined(TM2_CH2)
    TIMER_GPIO_PWM_P_INIT(TM2_CH2, TIMER2, TIMER_CH_2);
#endif

#if defined(TM2_CH3)
    TIMER_GPIO_PWM_P_INIT(TM2_CH3, TIMER2, TIMER_CH_3);
#endif

    timer_enable(TIMER2);

#endif


#if USE_TIMER_3

    timer_init_config(RCU_TIMER3, TIMER3, TM3_PRESCALER, TM3_PERIOD);

#if defined(TM3_CH0)
    TIMER_GPIO_PWM_P_INIT(TM3_CH0, TIMER3, TIMER_CH_0);
#endif

#if defined(TM3_CH1)
    TIMER_GPIO_PWM_P_INIT(TM3_CH1, TIMER3, TIMER_CH_1);
#endif

#if defined(TM3_CH2)
    TIMER_GPIO_PWM_P_INIT(TM3_CH2, TIMER3, TIMER_CH_2);
#endif

#if defined(TM3_CH3)
    TIMER_GPIO_PWM_P_INIT(TM3_CH3, TIMER3, TIMER_CH_3);
#endif

    timer_enable(TIMER3);

#endif


#if USE_TIMER_4

    timer_init_config(RCU_TIMER4, TIMER4, TM4_PRESCALER, TM4_PERIOD);

#if defined(TM4_CH0)
    TIMER_GPIO_PWM_P_INIT(TM4_CH0, TIMER4, TIMER_CH_0);
#endif

#if defined(TM4_CH1)
    TIMER_GPIO_PWM_P_INIT(TM4_CH1, TIMER4, TIMER_CH_1);
#endif

#if defined(TM4_CH2)
    TIMER_GPIO_PWM_P_INIT(TM4_CH2, TIMER4, TIMER_CH_2);
#endif

#if defined(TM4_CH3)
    TIMER_GPIO_PWM_P_INIT(TM4_CH3, TIMER4, TIMER_CH_3);
#endif

    timer_enable(TIMER4);

#endif


#if USE_TIMER_5
    timer_init_config(RCU_TIMER5, TIMER5, TM5_PRESCALER, TM5_PERIOD);
    
    // ============= NVIC ================
// 配置优先级 nvic_irq_enable
    nvic_irq_enable(TIMER5_DAC_IRQn,2,2);
// 启动中断 timer_interrupt_enable
    timer_interrupt_enable(TIMER5,TIMER_INT_UP);
    timer_enable(TIMER5);
#endif


#if USE_TIMER_6

    // TIMER6 一般是基本定时器，无 PWM 输出
    timer_init_config(RCU_TIMER6, TIMER6, TM6_PRESCALER, TM6_PERIOD);
		
		    // 新增：开启 TIMER5 更新中断
    timer_interrupt_enable(TIMER6, TIMER_INT_UP);
    nvic_irq_enable(TIMER6_IRQn, 2, 0);
    timer_enable(TIMER6);

#endif


#if USE_TIMER_7

    timer_init_config(RCU_TIMER7, TIMER7, TM7_PRESCALER, TM7_PERIOD);

#if defined(TM7_CH0_ON) && defined(TM7_CH0_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM7_CH0_ON, TM7_CH0_OP, TIMER7, TIMER_CH_0);
#elif defined(TM7_CH0_OP)
    TIMER_GPIO_PWM_P_INIT(TM7_CH0_OP, TIMER7, TIMER_CH_0);
#elif defined(TM7_CH0_ON)
    TIMER_GPIO_PWM_N_INIT(TM7_CH0_ON, TIMER7, TIMER_CH_0);
#endif

#if defined(TM7_CH1_ON) && defined(TM7_CH1_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM7_CH1_ON, TM7_CH1_OP, TIMER7, TIMER_CH_1);
#elif defined(TM7_CH1_OP)
    TIMER_GPIO_PWM_P_INIT(TM7_CH1_OP, TIMER7, TIMER_CH_1);
#elif defined(TM7_CH1_ON)
    TIMER_GPIO_PWM_N_INIT(TM7_CH1_ON, TIMER7, TIMER_CH_1);
#endif

#if defined(TM7_CH2_ON) && defined(TM7_CH2_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM7_CH2_ON, TM7_CH2_OP, TIMER7, TIMER_CH_2);
#elif defined(TM7_CH2_OP)
    TIMER_GPIO_PWM_P_INIT(TM7_CH2_OP, TIMER7, TIMER_CH_2);
#elif defined(TM7_CH2_ON)
    TIMER_GPIO_PWM_N_INIT(TM7_CH2_ON, TIMER7, TIMER_CH_2);
#endif

#if defined(TM7_CH3_ON) && defined(TM7_CH3_OP)
    TIMER_GPIO_PWM_BOTH_INIT(TM7_CH3_ON, TM7_CH3_OP, TIMER7, TIMER_CH_3);
#elif defined(TM7_CH3_OP)
    TIMER_GPIO_PWM_P_INIT(TM7_CH3_OP, TIMER7, TIMER_CH_3);
#elif defined(TM7_CH3_ON)
    TIMER_GPIO_PWM_N_INIT(TM7_CH3_ON, TIMER7, TIMER_CH_3);
#endif

    timer_advanced_output_config(TIMER7);
    timer_enable(TIMER7);

#endif


#if USE_TIMER_8

    timer_init_config(RCU_TIMER8, TIMER8, TM8_PRESCALER, TM8_PERIOD);

#if defined(TM8_CH0)
    TIMER_GPIO_PWM_P_INIT(TM8_CH0, TIMER8, TIMER_CH_0);
#endif

#if defined(TM8_CH1)
    TIMER_GPIO_PWM_P_INIT(TM8_CH1, TIMER8, TIMER_CH_1);
#endif

    timer_enable(TIMER8);

#endif


#if USE_TIMER_9

    timer_init_config(RCU_TIMER9, TIMER9, TM9_PRESCALER, TM9_PERIOD);

#if defined(TM9_CH0)
    TIMER_GPIO_PWM_P_INIT(TM9_CH0, TIMER9, TIMER_CH_0);
#endif

    timer_enable(TIMER9);

#endif


#if USE_TIMER_10

    timer_init_config(RCU_TIMER10, TIMER10, TM10_PRESCALER, TM10_PERIOD);

#if defined(TM10_CH0)
    TIMER_GPIO_PWM_P_INIT(TM10_CH0, TIMER10, TIMER_CH_0);
#endif

    timer_enable(TIMER10);

#endif


#if USE_TIMER_11

    timer_init_config(RCU_TIMER11, TIMER11, TM11_PRESCALER, TM11_PERIOD);

#if defined(TM11_CH0)
    TIMER_GPIO_PWM_P_INIT(TM11_CH0, TIMER11, TIMER_CH_0);
#endif

#if defined(TM11_CH1)
    TIMER_GPIO_PWM_P_INIT(TM11_CH1, TIMER11, TIMER_CH_1);
#endif

    timer_enable(TIMER11);

#endif


#if USE_TIMER_12

    timer_init_config(RCU_TIMER12, TIMER12, TM12_PRESCALER, TM12_PERIOD);

#if defined(TM12_CH0)
    TIMER_GPIO_PWM_P_INIT(TM12_CH0, TIMER12, TIMER_CH_0);
#endif

    timer_enable(TIMER12);

#endif


#if USE_TIMER_13

    timer_init_config(RCU_TIMER13, TIMER13, TM13_PRESCALER, TM13_PERIOD);

#if defined(TM13_CH0)
    TIMER_GPIO_PWM_P_INIT(TM13_CH0, TIMER13, TIMER_CH_0);
#endif

    timer_enable(TIMER13);

#endif
}

#if USE_TIMER_5

void TIMER5_DAC_IRQHandler() {
    if(SET==timer_interrupt_flag_get(TIMER5,TIMER_INT_UP)) {
        timer_interrupt_flag_clear(TIMER5,TIMER_INT_UP);
			  TIMER5_irq_callback();
    }
}

#endif

#if USE_TIMER_6

void TIMER6_IRQHandler() {
    if(SET==timer_interrupt_flag_get(TIMER6,TIMER_INT_UP)) {
        timer_interrupt_flag_clear(TIMER6,TIMER_INT_UP);
			  TIMER6_irq_callback();
    }
}

#endif

/**
 * @brief 启动 TIMER
 */
void TIMER_start(uint32_t timer_periph) {
    timer_enable(timer_periph);
}


/**
 * @brief 停止 TIMER
 */
void TIMER_stop(uint32_t timer_periph) {
    timer_disable(timer_periph);
}


/**
 * @brief 直接更新比较值
 */
void TIMER_channel_pulse_update(uint32_t timer_periph,
                                uint16_t channel,
                                uint32_t t_period,
                                uint32_t pulse) {
    if (t_period == 0) {
        t_period = 1;
    }

    if (pulse > t_period) {
        pulse = t_period;
    }

    timer_channel_output_pulse_value_config(timer_periph, channel, pulse);
}


/**
 * @brief 更新指定 Timer 的 channel 占空比
 */
void TIMER_channel_duty_update(uint32_t timer_periph,
                               uint16_t channel,
                               uint32_t t_period,
                               float duty) {
    uint32_t pulse;

    if (t_period == 0) {
        t_period = 1;
    }

    // 占空比限幅
    if (duty < 0.0f) {
        duty = 0.0f;
    } else if (duty > 100.0f) {
        duty = 100.0f;
    }

    // 四舍五入
    pulse = (uint32_t)((duty * (float)t_period) / 100.0f + 0.5f);

    TIMER_channel_pulse_update(timer_periph, channel, t_period, pulse);
}
