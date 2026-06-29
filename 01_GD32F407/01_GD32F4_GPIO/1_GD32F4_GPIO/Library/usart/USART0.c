#include "USART0.h"
#include "string.h"


#define RX0_LENTH	128   // 数组总长度
static uint8_t RX0_Buffer[RX0_LENTH + 1]; // +1, 多留一个字符，放字符串结束符
static uint32_t rx0_cnt = 0; // 放了几个元素的个数，下标

// =============================== DMA ============================
// DMA发送功能开关 
#define USART0_TX_DMA_ENABLE      1
// DMA接收功能开关 
#define USART0_RX_DMA_ENABLE      1

#define USART0_RX_DMA_RCU         RCU_DMA1
#define USART0_RX_DMA_PERIPH_CH   DMA1, DMA_CH5
#define USART0_RX_DMA_PERIPH_SUB  DMA_SUBPERI4

// 外设的数据地址
#define USART0_DATA_ADDR          (uint32_t)&USART_DATA(USART0)

#define USART0_TX_DMA_RCU         RCU_DMA1
#define USART0_TX_DMA_PERIPH_CH   DMA1, DMA_CH7
#define USART0_TX_DMA_PERIPH_SUB  DMA_SUBPERI4

// 内存到外设拷贝， 方向：动态内存 -> USART0外设
// SRC: memory0_addr
// DST: periph_addr -> &USART_DATA(USART0)
// 外设：DMA1 CH7 子外设100
// 拷贝方向：内存到外设拷贝
// 配置源头地址：内存，以后再确定

//===========================================DMA_TX==============================
void DMA_tx_config() {
    //开启时钟
    rcu_periph_clock_enable(USART0_TX_DMA_RCU);
    // 重置
    dma_deinit(USART0_TX_DMA_PERIPH_CH);

    // 结构体参数初始化
    dma_single_data_parameter_struct init_struct;
    dma_single_data_para_struct_init(&init_struct);
    // ============ 内存到内存拷贝
    // 方向： 内存到外设
    init_struct.direction=DMA_MEMORY_TO_PERIPH;
	// 源头:内存（是动态变化不固定的）
 // init_struct.memory0_addr=USART0_DATA_ADDR;

    init_struct.memory_inc=DMA_MEMORY_INCREASE_ENABLE;
    //目的地：外设

    init_struct.periph_addr=USART0_DATA_ADDR;

    init_struct.periph_inc=DMA_PERIPH_INCREASE_DISABLE;

    // 搬运一个数据的大小
    init_struct.periph_memory_width=DMA_PERIPH_WIDTH_8BIT;
    // 搬运数据个数(因为是动态变化，所以不写)
    //init_struct.number=ARR_LEN;      
    // 优先级
    init_struct.priority=DMA_PRIORITY_HIGH;
    // DMA初始化， 设置搬运规则
    dma_single_data_mode_init(USART0_TX_DMA_PERIPH_CH,&init_struct);
		
		//设置子外设
		dma_channel_subperipheral_select(USART0_TX_DMA_PERIPH_CH,USART0_TX_DMA_PERIPH_SUB);
}

//===========================================DMA_RX==============================
void DMA_rx_config() {
    //开启时钟
    rcu_periph_clock_enable(USART0_RX_DMA_RCU);
    // 重置
    dma_deinit(USART0_RX_DMA_PERIPH_CH);

    // 结构体参数初始化
    dma_single_data_parameter_struct init_struct;
    dma_single_data_para_struct_init(&init_struct);
    // ============ 内存到内存拷贝
    // 方向： 内存到外设
    init_struct.direction=DMA_PERIPH_TO_MEMORY;
    // 源头：外设
    init_struct.periph_addr=USART0_DATA_ADDR;

    init_struct.periph_inc=DMA_PERIPH_INCREASE_DISABLE;
    //目的地：内存
    init_struct.memory0_addr=(uint32_t)RX0_Buffer;

    init_struct.memory_inc=DMA_MEMORY_INCREASE_ENABLE;


    // 搬运一个数据的大小
    init_struct.periph_memory_width=DMA_PERIPH_WIDTH_8BIT;
    // 搬运数据个数(因为是动态变化，所以不写)
    init_struct.number=RX0_LENTH;      
    // 优先级
    init_struct.priority=DMA_PRIORITY_HIGH;
		
		
    // DMA初始化， 设置搬运规则
    dma_single_data_mode_init(USART0_RX_DMA_PERIPH_CH,&init_struct);
		
		//设置子外设
		dma_channel_subperipheral_select(USART0_RX_DMA_PERIPH_CH,USART0_RX_DMA_PERIPH_SUB);
		
		dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
}
//=======================USART0_init初始化=========================
void USART0_init() {
	  #if USART0_TX_DMA_ENABLE
	  DMA_tx_config();
	  #endif
	  #if USART0_RX_DMA_ENABLE
	  DMA_rx_config();
	  #endif
    // PA9   串口0 TX 复用功能    AF7
    GPIO_output_af(USART0_TX_RCU, USART0_TX_PORT, USART0_TX_PIN, GPIO_OTYPE_PP, USART0_TX_AF);
    // PA10  串口0 RX 复用功能    AF7
    GPIO_output_af(USART0_RX_RCU, USART0_RX_PORT, USART0_RX_PIN, GPIO_OTYPE_PP, USART0_RX_AF);

    // =========== 串口配置
    // 时钟使能
    rcu_periph_clock_enable(RCU_USART0);
    // 重置串口(可选)
    usart_deinit(USART0);
    // 设置波特率(必须要配置)
    usart_baudrate_set(USART0, USART0_BAUDRATE);
    // 允许串口发送
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	 #if USART0_TX_DMA_ENABLE
	 // 允许串口DMA发送
	  usart_dma_transmit_config(USART0,USART_TRANSMIT_DMA_ENABLE);
		#endif
    // 允许串口接收
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
		#if USART0_RX_DMA_ENABLE
	  // 允许串口DMA接收
		usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
		#endif
    //=====================中断=====================

    //参数1：重点类型枚举常量      参数2：抢占优先级        参数三：响应优先级
    nvic_irq_enable( USART0_IRQn,USART0_PRIORITY);
    #if !USART0_RX_DMA_ENABLE
    //使能串口中断
    usart_interrupt_enable(USART0, USART_INT_RBNE);//非空缓存标志中断
		#endif
    usart_interrupt_enable(USART0, USART_INT_IDLE);//空闲标志中断
    // 使能串口
    usart_enable(USART0);
}

// 发送1个byte数据
void USART0_send_byte(uint8_t byte) {
	#if !USART0_TX_DMA_ENABLE
    usart_data_transmit(USART0,byte);
    while(RESET==(usart_flag_get(USART0,USART_FLAG_TBE)));
	#else
	USART0_send_data(&byte,1);
	#endif
}


// 发送多个byte数据
void USART0_send_data(uint8_t* data, uint32_t len) {
	if(data==NULL) return;
	#if !USART0_TX_DMA_ENABLE
    for(uint32_t i=0; i<len; i++) {
        USART0_send_byte(data[i]);
    }
	#else
	//指定DMA内存地址
    dma_memory_address_config(USART0_TX_DMA_PERIPH_CH,DMA_MEMORY_0,(uint32_t)data);
	//指定DMA发送个数
	dma_transfer_number_config(USART0_TX_DMA_PERIPH_CH,len);
	//通知DMA开始
	dma_channel_enable(USART0_TX_DMA_PERIPH_CH);
	//等待完成
	while(RESET==dma_flag_get(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF)){}
	//清楚标志位
  dma_flag_clear(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF);
	//可选：意思是等串口硬件发送完最后一个字节
	while(RESET==usart_flag_get(USART0, USART_FLAG_TC));
	usart_flag_clear(USART0, USART_FLAG_TC);
	#endif
}

// 发送字符串 (结尾标记\0)
void USART0_send_string(char *data) {
	if(data==NULL) return;
	#if !USART0_TX_DMA_ENABLE
    for(uint32_t i=0; data[i]!='\0'; i++) {
        USART0_send_byte(data[i]);
    }
	#else
	USART0_send_data((uint8_t*)data,strlen(data));
	#endif
}
#if USART0_PRINTF
// 重写fputc， printf即可使用
int fputc(int ch, FILE *f) {
    USART0_send_byte(ch); // 串口发送1个字节
    return ch;
}
#endif


void USART0_IRQHandler() {
	#if !USART0_RX_DMA_ENABLE
    if (SET==(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE))) {  //为1时，说明收到新数据，可读

        uint8_t data=usart_data_receive(USART0);
        if(rx0_cnt>=RX0_LENTH) rx0_cnt=0;
        RX0_Buffer[rx0_cnt++]=data;
    }
    if (SET==(usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))) {   //为1时，接收线空闲，帧结束
        usart_data_receive(USART0);
        RX0_Buffer[rx0_cnt]='\0';
#if USART0_RECV_CALLBACK
        USART0_on_recv(RX0_Buffer,rx0_cnt);
#endif
        rx0_cnt = 0;
    }
	#else
	  if (SET==(usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))) {   //为1时，接收线空闲，帧结束
        usart_data_receive(USART0);
			//禁用DMA
			dma_channel_disable(USART0_RX_DMA_PERIPH_CH);
			//获取剩余没有搬运个数
			uint32_t left_num=dma_transfer_number_get(USART0_RX_DMA_PERIPH_CH);
			//搬运个数=总个数-剩余没有搬运个数
			rx0_cnt=RX0_LENTH-left_num;
        RX0_Buffer[rx0_cnt]='\0';
			
#if USART0_RECV_CALLBACK
        USART0_on_recv(RX0_Buffer,rx0_cnt);
#endif
			
        rx0_cnt = 0;
			
		 dma_flag_clear(USART0_RX_DMA_PERIPH_CH, DMA_FLAG_FTF);
			//禁用DMA
		 dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
		
    }
	
	#endif
}
