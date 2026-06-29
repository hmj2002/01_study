#include "bsp_MatrixKeys.h"

// ---------- 行输出宏（逐行拉低扫描） ----------
#define ROW1_PORT   GPIOE
#define ROW1_PIN    GPIO_PIN_8
#define ROW2_PORT   GPIOE
#define ROW2_PIN    GPIO_PIN_9
#define ROW3_PORT   GPIOE
#define ROW3_PIN    GPIO_PIN_10
#define ROW4_PORT   GPIOE
#define ROW4_PIN    GPIO_PIN_11

// ---------- 列输入宏 ----------
#define COL1_PORT   GPIOE
#define COL1_PIN    GPIO_PIN_12
#define COL2_PORT   GPIOE
#define COL2_PIN    GPIO_PIN_13
#define COL3_PORT   GPIOE
#define COL3_PIN    GPIO_PIN_14
#define COL4_PORT   GPIOE
#define COL4_PIN    GPIO_PIN_15

// ---------- 电平宏 ----------
#define DOWN        0
#define UP          1

// ---------- 按键状态宏 ----------
#define IS_KEY_UP(r,c)       ((states >> (r*4+c) & 1) == 1)
#define IS_KEY_DOWN(r,c)     ((states >> (r*4+c) & 1) == 0)
#define SET_KEY_UP(r,c)      (states |= (1 << (r*4+c)))
#define SET_KEY_DOWN(r,c)    (states &= ~(1 << (r*4+c)))

// ---------- 16个按键状态位图，1=释放 0=按下 ----------
uint16_t states = 0xFFFF;

/* ============================================================
 *  GPIO 初始化：行=推挽输出，列=输入上拉
 * ============================================================ */
static void GPIO_config(void)
{
    // 行：推挽输出，默认全拉高（释放）
    GPIO_output(RCU_GPIOE, ROW1_PORT, ROW1_PIN, GPIO_OTYPE_PP, 1);
    GPIO_output(RCU_GPIOE, ROW2_PORT, ROW2_PIN, GPIO_OTYPE_PP, 1);
    GPIO_output(RCU_GPIOE, ROW3_PORT, ROW3_PIN, GPIO_OTYPE_PP, 1);
    GPIO_output(RCU_GPIOE, ROW4_PORT, ROW4_PIN, GPIO_OTYPE_PP, 1);

    // 列：输入上拉
    GPIO_input(RCU_GPIOE, COL1_PORT, COL1_PIN, GPIO_PUPD_PULLUP);
    GPIO_input(RCU_GPIOE, COL2_PORT, COL2_PIN, GPIO_PUPD_PULLUP);
    GPIO_input(RCU_GPIOE, COL3_PORT, COL3_PIN, GPIO_PUPD_PULLUP);
    GPIO_input(RCU_GPIOE, COL4_PORT, COL4_PIN, GPIO_PUPD_PULLUP);
}

/* ============================================================
 *  行输出：指定行拉低，其余拉高
 * ============================================================ */
static void row_out(uint8_t r)
{
    gpio_bit_write(ROW1_PORT, ROW1_PIN, (r == 0) ? RESET : SET);
    gpio_bit_write(ROW2_PORT, ROW2_PIN, (r == 1) ? RESET : SET);
    gpio_bit_write(ROW3_PORT, ROW3_PIN, (r == 2) ? RESET : SET);
    gpio_bit_write(ROW4_PORT, ROW4_PIN, (r == 3) ? RESET : SET);
}

/* ============================================================
 *  列输入：读取指定列电平
 * ============================================================ */
static uint8_t col_in(uint8_t c)
{
    if(c == 0) return gpio_input_bit_get(COL1_PORT, COL1_PIN);
    if(c == 1) return gpio_input_bit_get(COL2_PORT, COL2_PIN);
    if(c == 2) return gpio_input_bit_get(COL3_PORT, COL3_PIN);
    if(c == 3) return gpio_input_bit_get(COL4_PORT, COL4_PIN);
    return UP;  // 默认释放
}

/* ============================================================
 *  初始化
 * ============================================================ */
void MK_init(void)
{
    GPIO_config();
}

/* ============================================================
 *  扫描按键（逐行扫描）
 * ============================================================ */
void MK_scan(void)
{
    uint8_t r, c;

    for(r = 0; r < 4; r++) {
        row_out(r);          // 当前行拉低，其余拉高
        delay_1us(10);       // 消抖延时，等电平稳定

        for(c = 0; c < 4; c++) {
            uint8_t col_state = col_in(c);

            // 按下检测（从释放到按下）
            if(IS_KEY_UP(r, c) && col_state == DOWN) {
                SET_KEY_DOWN(r, c);
                #if MK_USE_DOWN
                MK_on_keydown(r, c);
                #endif
            }
            // 释放检测（从按下到释放）
            else if(IS_KEY_DOWN(r, c) && col_state == UP) {
                SET_KEY_UP(r, c);
                #if MK_USE_UP
                MK_on_keyup(r, c);
                #endif
            }
        }

        // 恢复该行高电平，避免影响下一行扫描
        // 下一次 row_out() 会覆盖，这里可选
    }

    // 扫描完所有行，全部拉高释放
    gpio_bit_write(ROW1_PORT, ROW1_PIN, SET);
    gpio_bit_write(ROW2_PORT, ROW2_PIN, SET);
    gpio_bit_write(ROW3_PORT, ROW3_PIN, SET);
    gpio_bit_write(ROW4_PORT, ROW4_PIN, SET);
}