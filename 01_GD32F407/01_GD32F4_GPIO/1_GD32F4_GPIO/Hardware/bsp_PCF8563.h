#ifndef __BSP_PCF8563_H__
#define __BSP_PCF8563_H__

#include "I2C0_hard.h"

#define  I2C_WriteNbyte(a,b,c,d)  I2C0_hard_write(a,b,c,d)
#define  I2C_ReadNbyte(a,b,c,d)   I2C0_hard_read(a,b,c,d)

#define 	PCF8563_USE_ALARM	0
#define 	PCF8563_USE_TIMER	0



// 函数声明
void PCF8563_on_alarm();
void PCF8563_on_timer();

// bsp_PCF8563_soft.h  顶部
#ifndef WRITE_BCD
#define WRITE_BCD(val)  ((val / 10) << 4) + (val % 10)
#endif

#ifndef READ_BCD
#define READ_BCD(val)   (val >> 4) * 10 + (val & 0x0F)
#endif

// 设备地址
#define		PCF8563_DEV_ADDR		0x51
// 存储地址(寄存器地址): 时间(秒)存储地址
#define		PCF8563_REG_SECOND		0x02

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Clock_t;

// PCF8563初始化
void PCF8563_init();

// 设置时间
void PCF8563_set_clock(Clock_t temp);

// 获取时间
void PCF8563_get_clock(Clock_t *temp);


//=============================闹钟
typedef struct {
    // 设置分\时\天\周，如果为-1，禁用此项
    char minute ;
    char hour ;
    char day ;
    char weekday;
} Alarm_t;

// 设置闹钟
void PCF8563_set_alarm(Alarm_t alarm);
// 启用闹钟
void PCF8563_enable_alarm();
// 禁用闹钟Alarm
void PCF8563_disable_alarm();
// 清理闹钟标记
void PCF8563_alarm_clear_flag();


//=============================定时器
// 国产芯片的HZ1有问题，不要使用，建议使用HZ64
typedef enum { HZ4096 = 0, HZ64 = 1, HZ1 = 2, HZ1_60 = 3} TimerFreq;

// 启动定时器
void PCF8563_enable_timer();
// 禁用定时器
void PCF8563_disable_timer();
// 清除定时器标志位
void PCF8563_clear_timer();
// 设置定时器，参数1：时钟频率 参数2：倒计时计算值，时间为：参数2/参数1
void PCF8563_set_timer(TimerFreq freq, uint8_t countdown);

#endif