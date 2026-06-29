/*!
    \file    MyCAN.h
    \brief   GD32F407 CAN 驱动 - STM32 风格平替版
    \version V1.0.0

    使用说明：
    1. 修改顶部宏定义即可适配不同引脚/波特率
    2. API 与 STM32 标准库完全兼容
    3. 必须外接 CAN 收发器（如 TJA1050/SN65HVD230）
*/

#ifndef __MYCAN_H
#define __MYCAN_H

#include "gd32f4xx.h"       /* GD32 标准库头文件 */
#include "gd32f4xx_can.h"   /* CAN 外设头文件 */
#include <stdint.h>         /* 标准整数类型 */

/* ============================================================
 *  【引脚配置宏】修改此处即可换引脚，无需改动 .c 文件
 * ============================================================ */

/* GPIO 时钟 - 对应 RCC_APB2PeriphClockCmd 中的 RCC_APB2Periph_GPIOx */
#define CAN_GPIO_RCU            RCU_GPIOD

/* GPIO 端口 - 对应 GPIO_Init 中的 GPIOx */
#define CAN_GPIO_PORT           GPIOD

/* TX 引脚 - CAN 发送引脚，GD32F407 CAN0 可选 PA12/PB13/PB9/PD1/PH13 */
#define CAN_TX_PIN              GPIO_PIN_1

/* RX 引脚 - CAN 接收引脚，GD32F407 CAN0 可选 PA11/PB12/PB8/PD0/PH14 */
#define CAN_RX_PIN              GPIO_PIN_0

/* 复用功能号 - PB12/PB13 使用 AF9，其他引脚可能不同，需查数据手册 */
#define CAN_GPIO_AF             GPIO_AF_9

/* CAN 外设选择 - CAN0 或 CAN1，GD32F407 有两个 CAN */
#define CAN_USED_PERIPH         CAN0

/* CAN 外设时钟 - 与 CAN_USED_PERIPH 对应 */
#define CAN_USED_RCU            RCU_CAN0

/* ============================================================
 *  【波特率配置宏】APB1 总线时钟 = 42MHz
 *  
 *  波特率计算公式：
 *  波特率 = APB1时钟 / ((1 + BS1 + BS2) * Prescaler)
 *  
 *  当前配置：250kbps
 *  42MHz / ((1 + 6 + 1) * 21) = 42M / 168 = 250k
 * ============================================================ */

/* 预分频器 - 取值范围 1~1024，值越大波特率越低 */
#define CAN_PRESCALER           21

/* 时间段 1 (BS1) - 传播段 + 相位缓冲段1，取值 1~16 Tq */
#define CAN_BS1                 CAN_BT_BS1_6TQ   /* 实际 = 6+1 = 7 Tq */

/* 时间段 2 (BS2) - 相位缓冲段2，取值 1~8 Tq */
#define CAN_BS2                 CAN_BT_BS2_1TQ   /* 实际 = 1+1 = 2 Tq */

/* 同步跳转宽度 (SJW) - 重同步时最多可调整的 Tq 数，取值 1~4 Tq */
#define CAN_SJW                 CAN_BT_SJW_2TQ   /* 实际 = 2+1 = 3 Tq */

/* ============================================================
 *  【工作模式配置】
 * ============================================================ */

/* 正常通信模式 - 需要接收发器和总线 */
#define CAN_WORKING_MODE        CAN_NORMAL_MODE

/* 其他可选模式（用于调试）：
 * CAN_LOOPBACK_MODE      - 回环模式：自发自收，无需收发器，用于测试代码
 * CAN_SILENT_MODE        - 静默模式：只接收不发送，用于监听总线
 * CAN_SILENT_LOOPBACK_MODE - 静默回环：内部回环，不影响总线
 */

/* ============================================================
 *  【函数声明】与 STM32 标准库 API 完全兼容
 * ============================================================ */

/**
 * @brief  初始化 CAN 外设和 GPIO
 * @param  无
 * @retval 无
 * @note   自动配置 GPIO 复用、CAN 波特率、过滤器（接收所有）
 */
void MyCAN_Init(void);

/**
 * @brief  发送 CAN 标准数据帧
 * @param  ID:     标准标识符 (0x000 ~ 0x7FF)
 * @param  Length: 数据长度 (0 ~ 8)
 * @param  Data:   数据缓冲区指针
 * @retval 无
 * @note   阻塞等待发送完成，超时自动退出
 */
void MyCAN_Transmit(uint32_t ID, uint8_t Length, uint8_t *Data);

/**
 * @brief  检查接收 FIFO 是否有数据
 * @param  无
 * @retval 1: 有数据待读取, 0: 无数据
 * @note   非阻塞查询，用于主循环轮询
 */
uint8_t MyCAN_ReceiveFlag(void);

/**
 * @brief  从接收 FIFO 读取一帧数据
 * @param  ID:     输出参数，接收到的标识符
 * @param  Length: 输出参数，接收到的数据长度
 * @param  Data:   输出参数，接收到的数据缓冲区（至少 8 字节）
 * @retval 无
 * @note   调用前建议先用 MyCAN_ReceiveFlag() 检查，否则可能读到旧数据
 */
void MyCAN_Receive(uint32_t *ID, uint8_t *Length, uint8_t *Data);













//=============================================使用示例================================================

///* ============================================================
// *  发送数据
// * ============================================================ */
//static uint32_t TxID = 0x777;       /* 发送标识符 */
//static uint8_t  TxLen = 4;          /* 数据长度 */
//static uint8_t  TxData[8] = {0x11, 0x22, 0x33, 0x44};  /* 发送数据 */

///* ============================================================
// *  接收数据
// * ============================================================ */
//static uint32_t RxID;               /* 接收标识符 */
//static uint8_t  RxLen;              /* 接收长度 */
//static uint8_t  RxData[8];          /* 接收数据 */


///* OLED 显示缓冲区 */
//static char oled_buf[32];
///* ============================================================
// *  OLED 刷新发送数据显示
// * ============================================================ */
//static void OLED_RefreshTx(void)
//{
//    /* 显示发送 ID */
//    sprintf(oled_buf, "%03X", TxID);
//    OLED_Display_GB2312_string(40, 0, (u8 *)oled_buf);

//    /* 显示发送数据 */
//    sprintf(oled_buf, "%02X%02X%02X%02X", TxData[0], TxData[1], TxData[2], TxData[3]);
//    OLED_Display_GB2312_string(40, 2, (u8 *)oled_buf);
//}

///* ============================================================
// *  OLED 刷新接收数据显示
// * ============================================================ */
//static void OLED_RefreshRx(void)
//{
//    /* 显示接收 ID */
//    sprintf(oled_buf, "%03X", RxID);
//    OLED_Display_GB2312_string(40, 4, (u8 *)oled_buf);


//    /* 显示接收数据 */
//    sprintf(oled_buf, "%02X%02X%02X%02X", RxData[0], RxData[1], RxData[2], RxData[3]);
//    OLED_Display_GB2312_string(40, 6, (u8 *)oled_buf);
//}


//void USART0_on_recv(uint8_t* data, uint32_t len) {
//    printf("recv[%d] = %s\n", len, data);
//}

//void Keys_on_keydown(uint8_t index) {
//    if (index == 1) {
//        /* 数据自增 */
//        TxData[0]++;
//        TxData[1]++;
//        TxData[2]++;
//        TxData[3]++;

//        /* 发送 */
//        MyCAN_Transmit(TxID, TxLen, TxData);

//        /* 串口调试 */
//        printf("TX: ID=0x%03X, Len=%d, Data=%02X %02X %02X %02X\r\n",
//               TxID, TxLen, TxData[0], TxData[1], TxData[2], TxData[3]);

//        /* OLED 刷新发送显示 */
//        OLED_RefreshTx();
//    }
//}

//int main(void) {
//    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
//    systick_config();
//    USART0_init();
//    PCF8563_init();
//    DHT11_init();
//    MyCAN_Init();     
//    Keys_init();	
//    OLED_Init();
//    OLED_ColorTurn(0);
//    OLED_DisplayTurn(0);
//	

//    /* 第0行: 发送 ID */
//    OLED_Display_GB2312_string(0, 0, (u8 *)"TxID:");
//    OLED_RefreshTx();   /* 初始显示 */

//    /* 第2行: 发送数据 */
//    OLED_Display_GB2312_string(0, 2, (u8 *)"TxDt:");

//    /* 第4行: 接收 ID */
//    OLED_Display_GB2312_string(0, 4, (u8 *)"RxID:");


//    /* 第7行: 接收数据 */
//    OLED_Display_GB2312_string(0, 6, (u8 *)"RxDt:");

//    printf("Init OK. Press KEY1 to send\r\n");

//    /* ========== 主循环 ========== */
//    while (1) {
//        /* 扫描按键（10ms 一次，响应及时） */
//        Keys_scan();

//        /* 检查 CAN 接收 */
//        if (MyCAN_ReceiveFlag()) {
//            MyCAN_Receive(&RxID, &RxLen, RxData);

//            /* 串口调试 */
//            printf("RX: ID=0x%03X, Len=%d, Data=%02X %02X %02X %02X\r\n",
//                   RxID, RxLen, RxData[0], RxData[1], RxData[2], RxData[3]);

//            /* OLED 刷新接收显示 */
//            OLED_RefreshRx();
//        }

//        /* 10ms 延时，保证按键灵敏 + 轮询频率足够 */
//        delay_1ms(10);
//    }
//}


#endif /* __MYCAN_H */