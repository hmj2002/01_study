#ifndef __USART2_H__
#define __USART2_H__

#include "gd32f4xx.h"
#include "systick.h"
#include "gpio_cfg.h"
#include <stdio.h>

// 功能开关, printf配置开关
#define USART2_PRINTF			0
// 开关打开为1，同时，在合适位置定义函数void USART2_on_recv(uint8_t* data, uint32_t len)
#define USART2_RECV_CALLBACK	0

#if USART2_RECV_CALLBACK
// 收到串口0数据，回调函数
void USART2_on_recv(uint8_t* data, uint32_t len);
#endif

// PA9 	USART2_TX	AF7
#define USART2_TX_RCU 	RCU_GPIOC
#define USART2_TX_PORT 	GPIOC
#define USART2_TX_PIN 	GPIO_PIN_10
#define USART2_TX_AF 	GPIO_AF_7

// PA10 	USART2_RX	AF7
#define USART2_RX_RCU 	RCU_GPIOC
#define USART2_RX_PORT 	GPIOC
#define USART2_RX_PIN 	GPIO_PIN_11
#define USART2_RX_AF 	GPIO_AF_7

// 波特率
#define USART2_BAUDRATE 115200UL

// 优先级
#define USART2_PRIORITY	2,2

// 初始化
void USART2_init(); 

// 发送1个byte数据
void USART2_send_byte(uint8_t byte);

// 发送多个byte数据
void USART2_send_data(uint8_t* data, uint32_t len);

// 发送字符串 (结尾标记\0)
void USART2_send_string(char *data);

#endif