#include  "bsp_voltage.h"

// 初始化
void Voltage_init() {
    printf("Voltage_init\n");

}

// 获取电压
float Voltage_get() {
	
    uint16_t adc=ADC0_get(1);
    float vol=adc*3.3/4096;

    return vol*16;
}