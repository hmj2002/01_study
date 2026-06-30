#include "bsp_adkey.h"
#include <stdio.h>

#define ADKEY_RCU           RCU_GPIOA
#define ADKEY_PIN           GPIOA, GPIO_PIN_2


void ADKey_init(void)
{
    GPIO_analog(ADKEY_RCU, ADKEY_PIN);
}

/******************** 按键参数区 ********************/
/*
 * 当前版本针对“单键体验优化”
 * 默认只识别 KEY1 / KEY2 / KEY3 三个单键事件
 * 不对组合键做 click / double / long 判定
 * 这样稳定性和体验更好
 */

// 这三个值建议按你实际板子再微调
#define KEY1_VALUE          950
#define KEY2_VALUE          2048
#define KEY3_VALUE          3140

// 松手时 ADC 通常接近满量程
#define KEY_UP_MIN          3900

// 按下识别窗口
#define KEY_RANGE_DOWN      35

// 已经按住后的保持窗口，比 DOWN 稍宽一点，减少抖动误判
#define KEY_RANGE_HOLD      55

// 扫描周期：你当前任务里 ADKey_scan() 是每 1ms 调一次
#define ADKEY_SCAN_PERIOD_MS    1

// 消抖时间
#define ADKEY_DEBOUNCE_MS       12

// 长按触发时间
#define ADKEY_LONG_MS           600

// 双击最大间隔（第一次松手后到第二次松手完成）
#define ADKEY_DOUBLE_MS         250

// 长按连发周期（长按触发后，每隔多久继续回调一次）
#define ADKEY_REPEAT_MS         150

#define ADKEY_DEBOUNCE_TICKS    (ADKEY_DEBOUNCE_MS / ADKEY_SCAN_PERIOD_MS)
#define ADKEY_LONG_TICKS        (ADKEY_LONG_MS / ADKEY_SCAN_PERIOD_MS)
#define ADKEY_DOUBLE_TICKS      (ADKEY_DOUBLE_MS / ADKEY_SCAN_PERIOD_MS)
#define ADKEY_REPEAT_TICKS      (ADKEY_REPEAT_MS / ADKEY_SCAN_PERIOD_MS)

#define ADKEY_NONE              0
#define ADKEY_INVALID           0xFF

/******************** 工具函数 ********************/
static uint8_t ADKey_is_in(uint16_t value, uint16_t center, uint16_t range)
{
    return ((int32_t)value >= ((int32_t)center - (int32_t)range)) &&
           ((int32_t)value <= ((int32_t)center + (int32_t)range));
}

/*
 * 单键解码：
 * 返回值：
 * 0   无按键
 * 1   KEY1
 * 2   KEY2
 * 3   KEY3
 * 0xFF 无效过渡区，忽略本次采样
 */
static uint8_t ADKey_decode_value(uint16_t value, uint8_t stable_key)
{
    // 如果当前已有稳定按键，先用更宽的 HOLD 窗口保持
    if (stable_key == 1 && ADKey_is_in(value, KEY1_VALUE, KEY_RANGE_HOLD)) return 1;
    if (stable_key == 2 && ADKey_is_in(value, KEY2_VALUE, KEY_RANGE_HOLD)) return 2;
    if (stable_key == 3 && ADKey_is_in(value, KEY3_VALUE, KEY_RANGE_HOLD)) return 3;

    // 再按普通 DOWN 窗口识别
    if (ADKey_is_in(value, KEY1_VALUE, KEY_RANGE_DOWN)) return 1;
    if (ADKey_is_in(value, KEY2_VALUE, KEY_RANGE_DOWN)) return 2;
    if (ADKey_is_in(value, KEY3_VALUE, KEY_RANGE_DOWN)) return 3;

    // 无按键
    if (value >= KEY_UP_MIN) return ADKEY_NONE;

    // 中间过渡区，忽略
    return ADKEY_INVALID;
}

/******************** 事件状态机 ********************/
static uint8_t  s_stable_key      = ADKEY_NONE;     // 当前稳定键值
static uint8_t  s_raw_last_key    = ADKEY_INVALID;  // 上次原始键值
static uint16_t s_debounce_cnt    = 0;

// IIR 滤波值
static uint16_t s_adc_filtered    = 0;
static uint8_t  s_filter_inited   = 0;

// 当前按下过程
static uint16_t s_press_ticks     = 0;
static uint8_t  s_long_reported   = 0;
static uint16_t s_repeat_ticks    = 0;

// 单击 / 双击等待
static uint8_t  s_click_wait_key  = ADKEY_NONE;
static uint16_t s_click_wait_ticks= 0;

uint8_t ADKey_get_state(void)
{
    return s_stable_key;
}

static void ADKey_clear_click_wait(void)
{
    s_click_wait_key = ADKEY_NONE;
    s_click_wait_ticks = 0;
}

static void ADKey_start_press(uint8_t key)
{
    // 若等待的是别的键的单击，这里可直接结算那个单击
    if (s_click_wait_key != ADKEY_NONE && s_click_wait_key != key) {
        ADCKey_on_click(s_click_wait_key);
        ADKey_clear_click_wait();
    }

    s_press_ticks = 0;
    s_long_reported = 0;
    s_repeat_ticks = 0;

    ADCKey_on_down(key);
}

static void ADKey_end_press(uint8_t key)
{
    ADCKey_on_up(key);

    // 长按已经触发过，则不再触发 click / double
    if (s_long_reported) {
        if (s_click_wait_key == key) {
            ADKey_clear_click_wait();
        }
        s_press_ticks = 0;
        s_long_reported = 0;
        s_repeat_ticks = 0;
        return;
    }

    // 如果当前已经有同一个键在等待双击，则判定为双击
    if (s_click_wait_key == key && s_click_wait_ticks < ADKEY_DOUBLE_TICKS) {
        ADCKey_on_double_click(key);
        ADKey_clear_click_wait();
    } else {
        // 否则开始等待单击确认
        if (s_click_wait_key != ADKEY_NONE && s_click_wait_key != key) {
            ADCKey_on_click(s_click_wait_key);
        }
        s_click_wait_key = key;
        s_click_wait_ticks = 0;
    }

    s_press_ticks = 0;
    s_long_reported = 0;
    s_repeat_ticks = 0;
}

static void ADKey_on_stable_change(uint8_t old_key, uint8_t new_key)
{
    if (old_key != ADKEY_NONE) {
        ADKey_end_press(old_key);
    }

    if (new_key != ADKEY_NONE) {
        ADKey_start_press(new_key);
    }
}

static void ADKey_process_timers(void)
{
    // 处理当前按下中的长按 / 连发
    if (s_stable_key != ADKEY_NONE) {
        if (s_press_ticks < 0xFFFF) {
            s_press_ticks++;
        }

        if (!s_long_reported && s_press_ticks >= ADKEY_LONG_TICKS) {
            s_long_reported = 1;

            // 一旦进入长按，取消该键的单击/双击等待
            if (s_click_wait_key == s_stable_key) {
                ADKey_clear_click_wait();
            }

            ADCKey_on_long_click(s_stable_key);
            s_repeat_ticks = 0;
        } else if (s_long_reported) {
            if (s_repeat_ticks < 0xFFFF) {
                s_repeat_ticks++;
            }

            if (s_repeat_ticks >= ADKEY_REPEAT_TICKS) {
                s_repeat_ticks = 0;
                ADCKey_on_long_repeat(s_stable_key);
            }
        }
    }
    // 处理单击等待超时
    else if (s_click_wait_key != ADKEY_NONE) {
        if (s_click_wait_ticks < 0xFFFF) {
            s_click_wait_ticks++;
        }

        if (s_click_wait_ticks >= ADKEY_DOUBLE_TICKS) {
            ADCKey_on_click(s_click_wait_key);
            ADKey_clear_click_wait();
        }
    }
}

/******************** 扫描主函数 ********************/
void ADKey_scan(void)
{
    uint16_t adc = ADC0_get(0);

    // IIR滤波：filtered = 3/4 * old + 1/4 * new
    if (!s_filter_inited) {
        s_adc_filtered = adc;
        s_filter_inited = 1;
    } else {
        s_adc_filtered = (uint16_t)((s_adc_filtered * 3 + adc) / 4);
    }

    // 先处理时间事件
    ADKey_process_timers();

    // 解码原始键值
    uint8_t raw_key = ADKey_decode_value(s_adc_filtered, s_stable_key);

    // 过渡区直接忽略，不参与消抖
    if (raw_key == ADKEY_INVALID) {
        return;
    }

    // 消抖
    if (raw_key != s_raw_last_key) {
        s_raw_last_key = raw_key;
        s_debounce_cnt = 0;
        return;
    }

    if (s_debounce_cnt < ADKEY_DEBOUNCE_TICKS) {
        s_debounce_cnt++;
        return;
    }

    // 稳定状态发生变化
    if (s_stable_key != raw_key) {
        uint8_t old_key = s_stable_key;
        s_stable_key = raw_key;
        ADKey_on_stable_change(old_key, s_stable_key);
    }
}

/******************** 默认弱回调，可在其他文件重写 ********************/
__attribute__((weak)) void ADCKey_on_down(uint8_t i)
{
//   printf("KEY%d DOWN\r\n", i);
}

__attribute__((weak)) void ADCKey_on_up(uint8_t i)
{
    //  printf("KEY%d UP\r\n", i);
}

__attribute__((weak)) void ADCKey_on_click(uint8_t i)
{
    printf("KEY%d CLICK\r\n", i);
}

__attribute__((weak)) void ADCKey_on_long_click(uint8_t i)
{
    printf("KEY%d LONG\r\n", i);
}

__attribute__((weak)) void ADCKey_on_double_click(uint8_t i)
{
    printf("KEY%d DOUBLE\r\n", i);
}

__attribute__((weak)) void ADCKey_on_long_repeat(uint8_t i)
{
    // 默认不打印，避免长按刷屏太厉害
    // printf("KEY%d REPEAT\r\n", i);
}
