#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__

#include "gpio_cfg.h"
#include "USART0.h"
#define DHT		GPIOB,GPIO_PIN_15	// DHT11引脚



void DHT11_init();
// 返回负数：获取数据失败， 返回0：获取数据成功
char DHT11_get_humidity_temperature(int *humidity, float *temperature);

#endif