#include "bsp_keys.h"

#define KEY_PORT        GPIOC
#define KEY_RCU         RCU_GPIOC
#define KEY_PULL_MODE   GPIO_PUPD_PULLUP

#define KEY_COUNT       4
#define KEY_ALL_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)

#define KEY_FILTER_CNT  2     // 2次一致才确认，若 scan 周期5ms，则消抖约10ms

#define DOWN            RESET
#define UP              SET

static const uint32_t s_key_pins[KEY_COUNT] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3
};

typedef struct {
    FlagStatus stable_state;   // 稳定状态
    FlagStatus sample_state;   // 最近一次采样状态
    uint8_t    filter_cnt;     // 消抖计数
} KeyState_t;

static KeyState_t s_keys[KEY_COUNT];

static inline void Keys_report_event(uint8_t index, FlagStatus state)
{
    if (state == DOWN) {
        Keys_on_keydown(index + 1);
    } else {
        Keys_on_keyup(index + 1);
    }
}

void Keys_init(void)
{
    GPIO_input(KEY_RCU, KEY_PORT, KEY_ALL_PINS, KEY_PULL_MODE);

    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        FlagStatus state = gpio_input_bit_get(KEY_PORT, s_key_pins[i]);
        s_keys[i].stable_state = state;
        s_keys[i].sample_state = state;
        s_keys[i].filter_cnt = 0;
    }
}

void Keys_scan(void)
{
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        FlagStatus raw_state = gpio_input_bit_get(KEY_PORT, s_key_pins[i]);

        if (raw_state == s_keys[i].sample_state) {
            if (s_keys[i].filter_cnt < KEY_FILTER_CNT) {
                s_keys[i].filter_cnt++;
            } else {
                if (raw_state != s_keys[i].stable_state) {
                    s_keys[i].stable_state = raw_state;
                    Keys_report_event(i, raw_state);
                }
            }
        } else {
            s_keys[i].sample_state = raw_state;
            s_keys[i].filter_cnt = 0;
        }
    }
}

FlagStatus Keys_get_state(uint8_t index)
{
    if (index == 0 || index > KEY_COUNT) {
        return UP;
    }

    return s_keys[index - 1].stable_state;
}

__attribute__((weak)) void Keys_on_keydown(uint8_t index)
{
}

__attribute__((weak)) void Keys_on_keyup(uint8_t index)
{
}
