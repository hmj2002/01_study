#include "rtc_time.h"

/* ============================================================
 *  RTC 时间操作模块 - 源文件
 * ============================================================ */

/* ---------- RTC 初始化函数 (时钟配置) ---------- */
void RTC_config(void) {
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
    rcu_bkp_reset_enable();
    rcu_bkp_reset_disable();
    rcu_osci_on(RTC_OSCILLATOR);
    while(SUCCESS != rcu_osci_stab_wait(RTC_OSCILLATOR)) {}
    rcu_rtc_clock_config(RTC_CLK_SOURCE);
    rcu_rtc_div_config(RTC_CLK_DIV);
    rcu_periph_clock_enable(RCU_RTC);
    while(SUCCESS != rtc_register_sync_wait());
}

/* ---------- RTC 写入时间 ---------- */
void RTC_write_time(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday,
                    uint8_t hour, uint8_t minute, uint8_t second) {
    rtc_parameter_struct ris = {0};

    ris.year        = WRITE_BCD(year);
    ris.month       = WRITE_BCD(month);
    ris.date        = WRITE_BCD(date);
    ris.day_of_week = WRITE_BCD(weekday);
    ris.hour        = WRITE_BCD(hour);
    ris.minute      = WRITE_BCD(minute);
    ris.second      = WRITE_BCD(second);
    ris.display_format = RTC_24HOUR;

    ris.factor_asyn = RTC_FACTOR_ASYN;
    ris.factor_syn  = RTC_FACTOR_SYN;

    rtc_init(&ris);
}

/* ---------- RTC 读取时间并打印 ---------- */
void RTC_read(void) {
    rtc_parameter_struct ris;
    rtc_current_time_get(&ris);
    printf("%02d-%02d-%02d\n", READ_BCD(ris.year) + 2000, READ_BCD(ris.month), READ_BCD(ris.date));
    printf("weekday: %02d\n", READ_BCD(ris.day_of_week));
    printf("%02d:%02d:%02d\n", READ_BCD(ris.hour), READ_BCD(ris.minute), READ_BCD(ris.second));
}

/* ---------- RTC 闹钟配置 ---------- */
void RTC_alarm_config(void) {
    #if ALARM_ENABLE
    rtc_alarm_struct alarm_config = {0};

    alarm_config.alarm_mask         = ALARM_MASK;
    alarm_config.weekday_or_date    = ALARM_DATE_OR_WEEKDAY;
    alarm_config.alarm_day          = WRITE_BCD(ALARM_DAY);
    alarm_config.alarm_hour         = WRITE_BCD(ALARM_HOUR);
    alarm_config.alarm_minute       = WRITE_BCD(ALARM_MINUTE);
    alarm_config.alarm_second       = WRITE_BCD(ALARM_SECOND);

    rtc_alarm_config(RTC_ALARM0, &alarm_config);

    exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_17);
    nvic_irq_enable(RTC_Alarm_IRQn, 2, 2);
    rtc_interrupt_enable(RTC_INT_ALARM0);
    rtc_alarm_enable(RTC_ALARM0);
    #endif
}

#if ALARM_ENABLE
/* ---------- 闹钟中断处理函数 ---------- */
void RTC_Alarm_IRQHandler(void) {
    if(RESET != exti_interrupt_flag_get(EXTI_17)) {
        exti_interrupt_flag_clear(EXTI_17);
        rtc_flag_clear(RTC_FLAG_ALRM0);
			
			  ALARM_on_recv();

    }
}
#endif



//======================================独立看门狗===================================
void FWDGT_config() {
	// 配置时钟源
	rcu_osci_on(RCU_IRC32K);
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K));
	
	/* 独立看门狗配置 fwdgt_config
		参数1：重载计数值(这个不是时间的值，是计数值，需要通过时间，计算出这个计数值)
			  0x0000 - 0x0FFF 12位向下递减计数器初始值， Max: 4095
		参数2：预分频系数，将32kHZ进行降频 IRC32
			  32000Hz / 32 = 1000Hz 每秒数1000次，每次1ms
			  32000Hz / 64 =  500Hz 每秒数 500次，每次2ms
		假设
		target_ms = 2000ms 超过此时间不喂狗(fwdgt_counter_reload)，触发看门狗重启
		PSC       = FWDGT_PSC_DIV32

		参数1(重载计数值) = target_ms / 记一次数的时长(ms)
				 = target_ms / (1000 / Freq)ms
				 = target_ms / (1000 / (32000Hz / PSC))ms
				 = target_ms / (1000 / (32000Hz / 32))ms
				 = target_ms / (1000 / 1000)ms
				 = 2000ms / 1ms
				 = 2000
				 
		参数1(重载计数值) = target_ms / 记一次数的时长(ms)
				 = target_ms / (1000 / Freq)ms
				 = target_ms / (1000 / (32000Hz / PSC))ms
				 = target_ms / (1000 / (32000Hz / 64))ms
				 = target_ms / (1000 / 500)ms
				 = 2000ms / 2ms
				 = 1000  // ==============>  计数次数
	*/
	//            2s时间,     32K做64分频
	fwdgt_config(1000,     FWDGT_PSC_DIV64);
	
	// 开启独立看门狗计数器 fwdgt_enable
	fwdgt_enable();
}

//======================================窗口看门狗===================================
void WWDGT_config() {
	// 初始化RCU时钟 rcu_periph_clock_enable
	rcu_periph_clock_enable(RCU_WWDGT);
// 窗口看门狗重置 wwdgt_deinit
	wwdgt_deinit();
/*
// 窗口看门狗配置 wwdgt_config
    参数1:  计数器值，用于设置看门狗计数器的初始值
    参数2： 窗口值，该值必须小于 counter 值
    参数3： 分频系数
        假设 WWDGT_CFG_PSC_DIV4
         Hz = (PCLK1 / 4096) / 4      每秒钟计数值
            = 42M / 4096 / 4
            = 2563.48 Hz

    求窗口时间范围：
    1/4分频系数 -> 1次计数耗时 ： 1000ms / 2563.48 Hz = 0.39ms
    窗口开始时间：(counter - window) * 1次计数耗时 = (127 - 80) * 0.39ms = 18.33ms
    窗口结束时间：(counter -   0x3F) * 1次计数耗时 = (127 - 63) * 0.39ms = 24.96ms

    需要在窗口时间内喂狗(wwdgt_counter_update)，否则会触发重启
    18.33ms < duration < 24.96ms
*/
   wwdgt_config(127,80,WWDGT_CFG_PSC_DIV4);
// 启动窗口看门狗 wwdgt_enable  
   wwdgt_enable();
}