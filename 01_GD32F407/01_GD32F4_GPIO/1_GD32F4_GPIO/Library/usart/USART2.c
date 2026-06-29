#include "USART2.h"

//=======================USART2_init初始化=========================
void USART2_init() {
    // PA9   串口0 TX 复用功能    AF7
    GPIO_output_af(USART2_TX_RCU, USART2_TX_PORT, USART2_TX_PIN, GPIO_OTYPE_PP, USART2_TX_AF);
    // PA10  串口0 RX 复用功能    AF7
    GPIO_output_af(USART2_RX_RCU, USART2_RX_PORT, USART2_RX_PIN, GPIO_OTYPE_PP, USART2_RX_AF);

    // =========== 串口配置
    // 时钟使能
    rcu_periph_clock_enable(RCU_USART2);
    // 重置串口(可选)
    usart_deinit(USART2);
    // 设置波特率(必须要配置)
    usart_baudrate_set(USART2, USART2_BAUDRATE);
    // 允许串口发送
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    // 允许串口接收
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    //=====================中断=====================

    //参数1：重点类型枚举常量      参数2：抢占优先级        参数三：响应优先级
    nvic_irq_enable( USART2_IRQn,USART2_PRIORITY);

    //使能串口中断
    usart_interrupt_enable(USART2, USART_INT_RBNE);//非空缓存标志中断
    usart_interrupt_enable(USART2, USART_INT_IDLE);//空闲标志中断
    // 使能串口
    usart_enable(USART2);
}

// 发送1个byte数据
void USART2_send_byte(uint8_t byte) {
    usart_data_transmit(USART2,byte);
    while(RESET==(usart_flag_get(USART2,USART_FLAG_TBE)));
}


// 发送多个byte数据
void USART2_send_data(uint8_t* data, uint32_t len) {
    for(uint32_t i=0; i<len; i++) {
        USART2_send_byte(data[i]);
    }
}

// 发送字符串 (结尾标记\0)
void USART2_send_string(char *data) {
    for(uint32_t i=0; data[i]!='\0'; i++) {
        USART2_send_byte(data[i]);
    }
}
#if USART2_PRINTF
// 重写fputc， printf即可使用
int fputc(int ch, FILE *f) {
    USART2_send_byte(ch); // 串口发送1个字节
    return ch;
}
#endif

#define RX2_LENTH	128   // 数组总长度
static uint8_t RX2_Buffer[RX2_LENTH + 1]; // +1, 多留一个字符，放字符串结束符
static uint32_t rx2_cnt = 0; // 放了几个元素的个数，下标

void USART2_IRQHandler() {
    if (SET==(usart_interrupt_flag_get(USART2,USART_INT_FLAG_RBNE))) {  //为1时，说明收到新数据，可读

        uint8_t data=usart_data_receive(USART2);
        if(rx2_cnt>=RX2_LENTH) rx2_cnt=0;
        RX2_Buffer[rx2_cnt++]=data;
    }
    if (SET==(usart_interrupt_flag_get(USART2,USART_INT_FLAG_IDLE))) {   //为1时，接收线空闲，帧结束
        usart_data_receive(USART2);
        RX2_Buffer[rx2_cnt]='\0';
			#if USART2_RECV_CALLBACK
			    USART2_on_recv(RX2_Buffer,rx2_cnt);
			#endif
        rx2_cnt = 0;
    }
}
