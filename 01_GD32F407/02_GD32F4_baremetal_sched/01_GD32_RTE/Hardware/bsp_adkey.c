#include "bsp_adkey.h"

#define ADKEY_RCU			RCU_GPIOA
#define ADKEY_PIN			GPIOA, GPIO_PIN_2

#define ADKEY_CHN			ADC_CHANNEL_2
#define ADKEY_SAMPLETIME	ADC_SAMPLETIME_84



#define ADC_RECV_LEN  1
// 用于接收两个通道的数值
uint16_t g_recv_buff[ADC_RECV_LEN];

#define DMA_CH  DMA1, DMA_CH0
#define DMA_PERIPH_ADDR   (uint32_t)(&ADC_RDATA(ADC0))
#define DMA_SUB_PERIPH    DMA_SUBPERI0

static void DMA_ADC_config() {
    //开启时钟
    rcu_periph_clock_enable(RCU_DMA1);
    // 重置
    dma_deinit(DMA_CH);

    // 结构体参数初始化
    dma_single_data_parameter_struct init_struct;
    dma_single_data_para_struct_init(&init_struct);
    // ============ 内存到内存拷贝
    // 方向： 内存到外设
    init_struct.direction=DMA_PERIPH_TO_MEMORY;
    // 源头：外设
    init_struct.periph_addr=DMA_PERIPH_ADDR;

    init_struct.periph_inc=DMA_PERIPH_INCREASE_DISABLE;
    //目的地：内存
    init_struct.memory0_addr=(uint32_t)g_recv_buff;

    init_struct.memory_inc=DMA_MEMORY_INCREASE_ENABLE;


    // 搬运一个数据的大小
    init_struct.periph_memory_width=DMA_PERIPH_WIDTH_16BIT;
    // 搬运数据个数(因为是动态变化，所以不写)
    init_struct.number=ADC_RECV_LEN;
    // 优先级
    init_struct.priority=DMA_PRIORITY_HIGH;
    //设置循环模式
    init_struct.circular_mode  = DMA_CIRCULAR_MODE_ENABLE;

    // DMA初始化， 设置搬运规则
    dma_single_data_mode_init(DMA_CH,&init_struct);

    //设置子外设
    dma_channel_subperipheral_select(DMA_CH,DMA_SUB_PERIPH);


    dma_channel_enable(DMA_CH);
}

static void ADC_init() {
    // 使能时钟 rcu_periph_clock_enable
    rcu_periph_clock_enable(RCU_ADC0);
    // 重置 adc_deinit
    adc_deinit();

    // ===================基本配置========================
    // 设置分频系数(84M/4=21M)  adc_clock_config
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
    // 设置分辨率 adc_resolution_config
    adc_resolution_config(ADC0,ADC_RESOLUTION_12B);
    // 设置数据对齐(右对齐) adc_data_alignment_config
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);

    // ===================模式配置========================
    // 设置同步模式(独立模式) adc_sync_mode_config
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    // 设置单次模式(连续转换) adc_special_function_config
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    // 设置扫描模式 adc_special_function_config
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);

    // ===================通道配置========================
    // 设置是否打开插入通道(不打开) adc_special_function_config
    adc_special_function_config(ADC0,ADC_INSERTED_CHANNEL_AUTO,DISABLE);
    // 设置转换通道个数(包括常规通道组和插入通道组) adc_channel_length_config

    adc_channel_length_config(ADC0,ADC_ROUTINE_CHANNEL,1);


    adc_routine_channel_config(ADC0,0,ADKEY_CHN,ADKEY_SAMPLETIME);

    // 使能ADC adc_enable
    adc_enable(ADC0);

    // 校准 adc_calibration_enable
    adc_calibration_enable(ADC0);

    adc_dma_request_after_last_enable(ADC0);

    adc_dma_mode_enable(ADC0);

    DMA_ADC_config();

    // 告诉ADC需要转换数据(使能触发)
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}


uint16_t ADC_get() {

//// 使能 ADC 的软件触发 adc_software_trigger_enable
//    adc_software_trigger_enable(ADC0,ADC_ROUTINE_CHANNEL);

//// 等待 ADC 转换完成 ADC_FLAG_EOC
//    while(RESET==adc_flag_get(ADC0,ADC_FLAG_EOC));

//// 清除 ADC 转换完成标志位
//    adc_flag_clear(ADC0,ADC_FLAG_EOC);

//============================================电位器=========================
// 读取 ADC 转换结果 adc_routine_data_read
    return g_recv_buff[0];
}



void ADKey_init() { // 初始化
    GPIO_analog(ADKEY_RCU,ADKEY_PIN);
    ADC_init();

}

#define SAMPLE_CNT 50
// 按键按下时adc的值
#define KEY1_VALUE	 			950
#define KEY2_VALUE	 			2048
#define KEY3_VALUE	 			3140
#define KEY1_KEY2_VALUE			770
#define KEY1_KEY3_VALUE			880
#define KEY2_KEY3_VALUE			1780
#define KEY1_KEY2_KEY3_VALUE	730
#define KEY_ALL_UP				4095

#define KEY1	(1 << 0)   // 0表示抬起，1表示按下
#define KEY2	(1 << 1)
#define KEY3	(1 << 2)

#define KEY_RANGE	20     // 判断是否在 ±20 范围内

// 判断是否在 ± KEY_RANGE 范围内
#define	IS_IN(a, r)		(((a) < ((r) + KEY_RANGE)) && ((a) > ((r) - KEY_RANGE)))

static uint8_t key_state=0;
static uint8_t last_state=0;
void ADKey_scan() { // 扫描
    static uint8_t i=0;
    static uint32_t num=0;
    static uint16_t value_min=4096;
    static uint16_t value_max=0;
	
    key_state=0;
	
    if(i<SAMPLE_CNT) {
        uint16_t adc=ADC_get();
        num+=adc;
        if(adc<value_min) value_min=adc;
        if(adc>value_max) value_max=adc;
        i++;
    } else {
        num =num-(value_min+value_max);
        uint16_t value=num/(SAMPLE_CNT-2);
        //printf("value=%d\n",value);

        i=0;
        num=0;
        value_min=4096;
        value_max=0;

        if(IS_IN(value,KEY1_VALUE)) {
					key_state|=KEY1;
        }else if(IS_IN(value,KEY2_VALUE)) {
					key_state|=KEY2;
        }else if(IS_IN(value,KEY3_VALUE)) {
					key_state|=KEY3;
        }else if(IS_IN(value,KEY1_KEY2_VALUE)) {
					key_state|=KEY1;	
					key_state|=KEY2;
        }else if(IS_IN(value,KEY1_KEY3_VALUE)) {
					key_state|=KEY1;	
					key_state|=KEY3;			
        }else if(IS_IN(value,KEY2_KEY3_VALUE)) {
					key_state|=KEY2;	
					key_state|=KEY3;
        }else if(IS_IN(value,KEY1_KEY2_KEY3_VALUE)) {
					key_state|=KEY1;	
					key_state|=KEY2;
					key_state|=KEY3;				
        }
		uint8_t diff=key_state^last_state;
		
		if(diff==0) return;
		last_state=key_state;
    if(diff & KEY1) printf("KEY1 %s\n", (key_state & KEY1) ? "DOWN" : "UP");
    if(diff & KEY2) printf("KEY2 %s\n", (key_state & KEY2) ? "DOWN" : "UP");
    if(diff & KEY3) printf("KEY3 %s\n", (key_state & KEY3) ? "DOWN" : "UP");
		}	
}
