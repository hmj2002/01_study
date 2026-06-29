#ifndef __RTC_TIME_H
#define __RTC_TIME_H

#include "gd32f4xx.h"
#include <stdio.h>

/* ============================================================
 *  RTC 时间操作模块 - 头文件
 *  
 *  【时间设置】通过函数参数传入，灵活调用
 *  【闹钟配置】通过修改下方宏定义配置
 *  【闹钟开关】通过 ALARM_ENABLE 宏控制
 * ============================================================ */

/* ---------- BCD 转换宏 ---------- */
// 十位取出左移4位 + 个位 (得到BCD数)
#define WRITE_BCD(val) 	((val / 10) << 4) + (val % 10)
// 将高4位乘以10 + 低四位 (得到10进制数)
#define READ_BCD(val) 	(val >> 4) * 10 + (val & 0x0F)

/* ============================================================
 *  【闹钟配置区】修改以下宏即可配置闹钟，无需改动函数体
 * ============================================================ */

// 闹钟总开关: 1=使能闹钟  0=禁用闹钟
#define ALARM_ENABLE        0                     //如果 使用闹钟功能，要在主函数使用回调函数： void ALARM_on_recv();

// 闹钟时间配置
#define ALARM_DAY           11      // 闹钟日期 (1-31)
#define ALARM_HOUR          0       // 闹钟小时 (0-23)
#define ALARM_MINUTE        0       // 闹钟分钟 (0-59)
#define ALARM_SECOND        0       // 闹钟秒钟 (0-59)

// 闹钟掩码选项 (取消注释所需行即可，只能选一个)
// #define ALARM_MASK          RTC_ALARM_DATE_MASK                 // 每天响（不匹配日期）
// #define ALARM_MASK          (RTC_ALARM_DATE_MASK | RTC_ALARM_HOUR_MASK)  // 每小时响（不匹配日期和小时）
#define ALARM_MASK          RTC_ALARM_NONE_MASK                  // 全匹配（指定日期时间响一次）

// 闹钟按日期还是星期匹配
#define ALARM_DATE_OR_WEEKDAY   RTC_ALARM_DATE_SELECTED     // 按日期匹配
// #define ALARM_DATE_OR_WEEKDAY   RTC_ALARM_WEEKDAY_SELECTED  // 按星期匹配

/* ============================================================
 *  【RTC硬件配置区】一般不需要修改
 * ============================================================ */
#define RTC_CLK_SOURCE      RCU_RTCSRC_HXTAL_DIV_RTCDIV     // RTC时钟源
#define RTC_CLK_DIV         RCU_RTC_HXTAL_DIV25              // 分频配置: 8M/25 = 320K
#define RTC_OSCILLATOR      RCU_LXTAL                        // 晶振选择

#define RTC_FACTOR_ASYN     0x7F     // 7位异步预分频器 (0x0 - 0x7F)
#define RTC_FACTOR_SYN      2499     // 15位同步预分频器 (0x0 - 0x7FFF)

#define RTC_BKP_FLAG        0x6666   // 备份寄存器标记值

/* ============================================================
 *  函数声明
 * ============================================================ */

// RTC 时钟初始化
void RTC_config(void);

// RTC 写入时间
// 参数: year(00-99), month(1-12), date(1-31), weekday(1-7),
//        hour(0-23), minute(0-59), second(0-59)
void RTC_write_time(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday,
                    uint8_t hour, uint8_t minute, uint8_t second);
/*===============================================下面可以用于短时间内上电不丢失，大约5分钟左右
    if (RTC_BKP0 != RTC_BKP_FLAG) {
        // 第一次上电，写入初始时间
        RTC_write_time(25, 10, 10, 5, 23, 59, 54);
        printf("写时间，只写1次\n");
        RTC_BKP0 = RTC_BKP_FLAG;
    }
*/


// RTC 读取时间并打印
void RTC_read(void);

// RTC 闹钟配置
void RTC_alarm_config(void);

void ALARM_on_recv();
 
//独立看门狗初始化                         如果使用需要在合适位置喂狗:fwdgt_counter_reload();      当前喂狗时间需要2S内
void FWDGT_config();

//窗口看门狗初始化                       	如果使用需要在合适位置喂狗：wwdgt_counter_update(127);  时间合适： elay_1ms(20); // 
void WWDGT_config();

#endif /* __RTC_TIME_H */