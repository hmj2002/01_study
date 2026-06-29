#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "USART0.h"
#include "EXTI.h"

uint8_t flag=0;

void USART0_on_recv(uint8_t* data, uint32_t len) {
//   printf("recv[%d] = %s\n", len, data);
    flag=data[0];
}

static void delay() {
    uint32_t i = 20000000;

    while(i--) {
        __NOP();
    }
}

void sleep_mode() {
    // 时钟使能 rcu_periph_clock_enable
    rcu_periph_clock_enable(RCU_PMU);
    // 睡眠模式 pmu_to_sleepmode
    pmu_to_sleepmode(WFI_CMD);
    printf("sleepmode\n");
}

void deep_sleep_mode() {
    // 时钟使能 rcu_periph_clock_enable
    rcu_periph_clock_enable(RCU_PMU);
    // 深度睡眠模式 pmu_to_deepsleepmode
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER,PMU_LOWDRIVER_ENABLE,WFI_CMD);
    // 把主频设置回来 SystemInit
    SystemInit();
    printf("deep_sleep_mode\n");
}

void standby_mode() {
    // 时钟使能 rcu_periph_clock_enable
	rcu_periph_clock_enable(RCU_PMU);
    // 清理待机模式标志(可选) pmu_flag_clear
	pmu_flag_clear(PMU_FLAG_RESET_STANDBY);
    // 启用唤醒按钮 pmu_wakeup_pin_enable
	pmu_wakeup_pin_enable();
    // 待机模式 pmu_to_standbymode
	pmu_to_standbymode();
    printf("standby_mode\n");
}

void EXTI1_on_trig() {
    printf("down\n");
}

int main(void) {
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();
    USART0_init();
    EXTI_Init();

    GPIO_output(RCU_GPIOB, GPIOB,GPIO_PIN_2, GPIO_OTYPE_PP,0);

    while(1)
    {
        if(flag!=0) {
            switch(flag) {
            case 1:
                sleep_mode();
                break;
            case 2:
                deep_sleep_mode();
                break;
            case 3:
                standby_mode();
                break;
            default:
                break;

            }
            flag=0;
        }
        gpio_bit_toggle(GPIOB,GPIO_PIN_2);
        delay_1ms(300);
    }
}