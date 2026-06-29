#include "I2C0_hard.h"

/* ============================================================
 *  I2C0 硬件实现 - 轮询模式
 * ============================================================ */

// 等待标记flag变为SET，成功return 0，超时return 1
static uint8_t I2C_wait(uint32_t flag) {
    uint32_t timeout = I2C0H_TIMEOUT;
    while(RESET == i2c_flag_get(I2C0, flag)) {
        if(timeout-- == 0) return 1;
    }
    return 0;
}

// 等待标记flag变为RESET，成功return 0，超时return 1
static uint8_t I2C_waitn(uint32_t flag) {
    uint32_t timeout = I2C0H_TIMEOUT;
    while(SET == i2c_flag_get(I2C0, flag)) {
        if(timeout-- == 0) return 1;
    }
    return 0;
}

// 错误处理：发送停止条件并清除错误标志
static void I2C_error_handler(void) {
    i2c_stop_on_bus(I2C0);
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);
    i2c_flag_clear(I2C0, I2C_FLAG_BERR);
    i2c_flag_clear(I2C0, I2C_FLAG_LOSTARB);
    i2c_flag_clear(I2C0, I2C_FLAG_OUERR);
}

/* ============================================================
 *  I2C0 硬件初始化
 * ============================================================ */
void I2C0_hard_init(void) {
    // GPIO 复用配置
    GPIO_output_af(I2C0H_SCL_RCU, I2C0H_SCL_PORT, I2C0H_SCL_PIN, GPIO_OTYPE_OD, I2C0H_AF);
    GPIO_output_af(I2C0H_SDA_RCU, I2C0H_SDA_PORT, I2C0H_SDA_PIN, GPIO_OTYPE_OD, I2C0H_AF);

    // I2C 时钟使能
    rcu_periph_clock_enable(RCU_I2C0);

    // 重置 I2C
    i2c_deinit(I2C0);

    // 配置I2C速度（标准模式100K或快速模式400K）
    i2c_clock_config(I2C0, I2C0H_SPEED, I2C_DTCY_2);

    // 配置I2C地址类型（7位地址）
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);

    // 启用 I2C
    i2c_enable(I2C0);

    // 收到数据自动回复ACK
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

/* ============================================================
 *  写数据到I2C设备指定寄存器
 * ============================================================ */
uint8_t I2C0_hard_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len) {
    /************* 1. 等待总线空闲 ***********************/
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;  // 总线一直忙

    /************* 2. 发送起始信号 ***********************/
    i2c_start_on_bus(I2C0);
    if(I2C_wait(I2C_FLAG_SBSEND)) {  // 等待起始信号发送完成
        I2C_error_handler();
        return 2;
    }

    /************* 3. 发送设备地址（写方向）**************/
    i2c_master_addressing(I2C0, addr << 1, I2C_TRANSMITTER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) {  // 等待地址发送完成
        I2C_error_handler();
        return 3;
    }
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);  // 清除ADDSEND标志

    /************* 4. 发送寄存器地址 **********************/
    if(I2C_wait(I2C_FLAG_TBE)) {  // 等待发送缓冲区空
        I2C_error_handler();
        return 4;
    }
    i2c_data_transmit(I2C0, reg);
    if(I2C_wait(I2C_FLAG_BTC)) {  // 等待字节发送完成
        I2C_error_handler();
        return 5;
    }

    /************* 5. 循环发送数据 **********************/
    for(uint32_t i = 0; i < len; i++) {
        if(I2C_wait(I2C_FLAG_TBE)) {  // 等待发送缓冲区空
            I2C_error_handler();
            return 6;
        }
        i2c_data_transmit(I2C0, dat[i]);
        if(I2C_wait(I2C_FLAG_BTC)) {  // 等待字节发送完成
            I2C_error_handler();
            return 7;
        }
    }

    /************* 6. 发送停止信号 **********************/
    i2c_stop_on_bus(I2C0);
    if(I2C_waitn(I2C_CTL0_STOP)) {  // 等待停止条件完成
        return 8;
    }

    return 0;
}

/* ============================================================
 *  从I2C设备指定寄存器读取数据
 * ============================================================ */
uint8_t I2C0_hard_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len) {
    /************* 1. 等待总线空闲 ***********************/
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    /************* 2. 第一阶段：写寄存器地址 *************/
    // 发送起始信号
    i2c_start_on_bus(I2C0);
    if(I2C_wait(I2C_FLAG_SBSEND)) {
        I2C_error_handler();
        return 2;
    }

    // 发送设备地址（写方向）
    i2c_master_addressing(I2C0, addr << 1, I2C_TRANSMITTER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) {
        I2C_error_handler();
        return 3;
    }
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    // 发送寄存器地址
    if(I2C_wait(I2C_FLAG_TBE)) {
        I2C_error_handler();
        return 4;
    }
    i2c_data_transmit(I2C0, reg);
    if(I2C_wait(I2C_FLAG_BTC)) {
        I2C_error_handler();
        return 5;
    }

    /************* 3. 第二阶段：读数据 ******************/
    // 重新发送起始信号（重复起始）
    i2c_start_on_bus(I2C0);
    if(I2C_wait(I2C_FLAG_SBSEND)) {
        I2C_error_handler();
        return 6;
    }

    // 发送设备地址（读方向）
    i2c_master_addressing(I2C0, addr << 1, I2C_RECEIVER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) {
        I2C_error_handler();
        return 7;
    }
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    // 根据读取长度配置ACK
    if(len == 1) {
        // 只读1个字节：立即禁用ACK，收到后NACK
        i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        i2c_stop_on_bus(I2C0);  // 提前发送停止条件
    } else if(len == 2) {
        // 读2个字节：配置ACK位置为下一个字节
        i2c_ackpos_config(I2C0, I2C_ACKPOS_NEXT);
        i2c_ack_config(I2C0, I2C_ACK_DISABLE);
    }
    // len >= 3 时保持ACK使能

    /************* 4. 循环读取数据 **********************/
    for(uint32_t i = 0; i < len; i++) {
        // 倒数第3个字节时：等待BTC，然后禁用ACK
        if(i == len - 3 && len > 2) {
            if(I2C_wait(I2C_FLAG_BTC)) {
                I2C_error_handler();
                return 8;
            }
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        }

        // 倒数第2个字节时：等待BTC，然后发送停止
        if(i == len - 2 && len > 1) {
            if(I2C_wait(I2C_FLAG_BTC)) {
                I2C_error_handler();
                return 9;
            }
            i2c_stop_on_bus(I2C0);
        }

        // 等待接收缓冲区非空
        if(I2C_wait(I2C_FLAG_RBNE)) {
            I2C_error_handler();
            return 10;
        }
        dat[i] = i2c_data_receive(I2C0);
    }

    /************* 5. 确保停止条件完成 ******************/
    if(I2C_waitn(I2C_CTL0_STOP)) {
        return 11;
    }

    // 恢复ACK使能，为下次通信做准备
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);

    return 0;
}