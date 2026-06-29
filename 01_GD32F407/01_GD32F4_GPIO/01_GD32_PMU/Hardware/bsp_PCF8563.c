#include 	"bsp_PCF8563.h"

// PCF8563初始化
void PCF8563_init() {
I2C0_hard_init();
}


// 设置时间
void PCF8563_set_clock(Clock_t temp) {
    uint8_t p[7] = {0};  // 写完整  年月日 星期几  时分秒
    // 秒的寄存器地址为： 0x02
    // 秒:  第0~3位记录个位，第4~6位记录十位
    //     十位                  个位
    p[0] = WRITE_BCD(temp.second);
    // 分: 第0~3位，保存个数，第4到6位，保存十位
    p[1] = WRITE_BCD(temp.minute);
    // 时：第0~3位，保存个数，第4到5位，保存十位
    p[2] = WRITE_BCD(temp.hour);
    // 日：第0~3位，保存个数，第4到5位，保存十位
    p[3] = WRITE_BCD(temp.day);
    // 周：第0~2位，保存个数
    p[4] = temp.weekday;
    // 月_世纪:  第0~3位记录个位，第4位记录十位，第7位为0，世纪数为20xx，为1，世纪数为21xx
    p[5] = WRITE_BCD(temp.month);
    // 月的第7位
    if (temp.year >= 2100) { // 第7位置1
        p[5] |= (1 << 7);
    }  // 第7位置0，不处理就是0

    // 年：第0~3位，保存个数，第4到7位，保存十位
    // 2025  ===> 25
    p[6] = WRITE_BCD(temp.year%100);
    I2C_WriteNbyte(PCF8563_DEV_ADDR, PCF8563_REG_SECOND, p, 7);
}

// 获取时间
void PCF8563_get_clock(Clock_t *temp) {
    uint8_t p[7] = {0};
    uint8_t flag;
    // 读时间
    I2C_ReadNbyte(PCF8563_DEV_ADDR, PCF8563_REG_SECOND, p, 7);

    // 10进制  用 16进制表示    低4位放个位     高4位放10位
    // 秒: 第0~3位记录个位，第4~6位记录十位
    temp->second = READ_BCD(p[0]);
    // 分: 第0~3位，保存个数，第4到6位，保存十位
    temp->minute = READ_BCD(p[1]);
    // 时：第0~3位，保存个数，第4到5位，保存十位
    temp->hour = READ_BCD(p[2]);
    // 日：第0~3位，保存个数，第4到5位，保存十位
    temp->day = READ_BCD(p[3]);
    // 周：第0~2位，保存个数
    temp->weekday = p[4]; // 如果是星期日，是0
    // 月_世纪:  第0~3位记录个位，第4位记录十位，第7位为0，世纪数为20xx，为1，世纪数为21xx
    // 处理第7位
    // 取出第7位
    flag = p[5] >> 7;
    // 第7位置0， 月的第7位，是年的标志位，不是月的有效数据
    p[5] &= ~(1 << 7);
    temp->month = READ_BCD(p[5]);
    // 年：第0~3位，保存个数，第4到7位，保存十位
    temp->year = READ_BCD(p[6]);
    if (flag == 1) temp->year += 2100;
    else temp->year += 2000;
}

// 设置闹钟
void PCF8563_set_alarm(Alarm_t alarm) {
    uint8_t p[4] = {0};
    //===================2.1 闹钟时间设置 寄存器地址 0x09
    // 分: 第0~3位,记录个数, 第4~6位记录十位, 第7位：置0启动, 置1禁用
    if (alarm.minute != -1) p[0] = WRITE_BCD(alarm.minute);
    else p[0] = 0x80;

    // 时: 第0~3位,记录个数, 第4~5位记录十位, 第7位：置0启动, 置1禁用
    p[1] = alarm.hour != -1 ? WRITE_BCD(alarm.hour) : 0x80 ;
    // 日: 第0~3位,记录个数, 第4~5位记录十位, 第7位：置0启动, 置1禁用
    p[2] = alarm.day != -1 ? WRITE_BCD(alarm.day) : 0x80 ;
    // 周: 第0~2位,记录个数, 第7位：置0启动, 置1禁用
    p[3] = alarm.weekday != -1 ? alarm.weekday : 0x80 ;
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x09, p, 4);
}

// 启用闹钟
void PCF8563_enable_alarm() {
    uint8_t cfg;
    //===================2.2 闹钟开启 寄存器地址 0x01
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位  第3位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 3);
    //c) 在原来配置基础上，启动闹钟，第1位：置0禁用，置1启动
    cfg |= (1 << 1);
    //d) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 禁用闹钟Alarm
void PCF8563_disable_alarm() {
    uint8_t cfg;
    //===================2.2 闹钟 寄存器地址 0x01
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位  第3位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 3);
    //c) 在原来配置基础上，禁用闹钟，第1位：置0禁用，置1启动
    cfg &= ~(1 << 1);
    //d) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 清理闹钟标记
void PCF8563_alarm_clear_flag() {
    uint8_t cfg;
    // 清除闹钟的标志位，才能重复触发闹钟
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位  第3位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 3);
    //c) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 启动定时器
void PCF8563_enable_timer() {
    uint8_t cfg;
    //============2 定时器开启  寄存器地址 0x01
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位，第2位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 2);
    //c) 在原来配置基础上，启动定时器，第0位：置0禁用，置1启用
    cfg |= (1 << 0);
    //d) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 禁用定时器
void PCF8563_disable_timer() {
    uint8_t cfg;
    //============2 定时器开启  寄存器地址 0x01
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位，第2位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 2);
    //c) 在原来配置基础上，启动定时器，第0位：置0禁用，置1启用
    cfg &= ~(1 << 0);
    //d) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 清除定时器标志位
void PCF8563_clear_timer() {
    uint8_t cfg;

    // 清除标志位，才能重复触发
    //a) 读原来的配置(不要乱改配置，只改自己的位，其它维持不变)
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    //b) 在原来配置的基础上，清除标志位，第2位：置0清除标志位，置1维持不变
    cfg &= ~(1 << 2);
    //c) 重新写入配置
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
}

// 设置定时器，参数1：时钟频率 参数2：倒计时计算值，时间为：参数2/参数1
void PCF8563_set_timer(TimerFreq freq, uint8_t countdown) {
    uint8_t p[2] = {0};

    //============1 定时器设置 寄存器地址 0x0e
    //a) 时钟频率  不建议用 1hz，有bug
    // 第7位为0，定时器禁用，第七位为1，定时器启用
    p[0] = freq + 0x80; // 64hz
    //b) 计数值(0~255) ===》时间为： 计数值/时钟频率
    p[1] = countdown; // 时间间隔：countdown / freq
    I2C_WriteNbyte(PCF8563_DEV_ADDR, 0x0e, p, 2);

}

void int3_callback() { // 外部中断3的回调
    uint8_t cfg;

    // 读取原来的配置
    I2C_ReadNbyte(PCF8563_DEV_ADDR, 0x01, &cfg, 1);
    // 如果是闹钟，第1位和第3位是1
    if ((cfg & 0x02) && (cfg & 0x08)) {
        // 清理闹钟标记
        PCF8563_alarm_clear_flag();

        // 调用
#if PCF8563_USE_ALARM
        PCF8563_on_alarm();
#endif
    }

    // 如果是定时器，第0位和第2位是1
    if ((cfg & 0x01) && (cfg & 0x04)) {
        // 清除定时器标志位
        PCF8563_clear_timer();

        // 调用
#if PCF8563_USE_TIMER
        PCF8563_on_timer();
#endif
    }
}
