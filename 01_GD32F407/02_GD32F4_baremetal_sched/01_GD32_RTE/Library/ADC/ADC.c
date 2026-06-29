#include "ADC.h"

#if ADC_MODE_SINGLE
//===========================================单通道初始化===============================================
void ADC_init() {
	
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
    // 设置单次模式(禁用连续转换) adc_special_function_config
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,DISABLE);
    // 设置非扫描模式 adc_special_function_config
    adc_special_function_config(ADC0,ADC_SCAN_MODE,DISABLE);
    
    // ===================通道配置========================
    // 设置是否打开插入通道(不打开) adc_special_function_config
    adc_special_function_config(ADC0,ADC_INSERTED_CHANNEL_AUTO,DISABLE);
    // 设置转换通道个数(包括常规通道组和插入通道组) adc_channel_length_config
    adc_channel_length_config(ADC0,ADC_ROUTINE_CHANNEL,1);
		
    // 使能ADC adc_enable
    adc_enable(ADC0);
    // 内部校准(需要delay等待) 
    delay_1ms(1);
    // 校准 adc_calibration_enable
    adc_calibration_enable(ADC0);
}

//=======================================================提供范例=========================================

void ADC_temp() {
 // 设置转换哪一个通道以及所处序列位置 adc_routine_channel_config
 // 温度传感器需要更长的采样时间，因为内部信号源阻抗较高，需要更多时间充电采样电容
 // 确保信号在采样期间能稳定到足够精度 >= ADC_SAMPLETIME_84
    adc_routine_channel_config(ADC0,0,ADC_CHANNEL_16,ADC_SAMPLETIME_84);
 //    // 使能16到18内部通道(温度和参考电压通道) adc_channel_16_to_18
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);
// 使能 ADC 的软件触发 adc_software_trigger_enable
	adc_software_trigger_enable(ADC0,ADC_ROUTINE_CHANNEL);
	
// 等待 ADC 转换完成 ADC_FLAG_EOC
	while(RESET==adc_flag_get(ADC0,ADC_FLAG_EOC));

// 清除 ADC 转换完成标志位
	adc_flag_clear(ADC0,ADC_FLAG_EOC);
	
// 读取 ADC 转换结果 adc_routine_data_read
	uint16_t adc=adc_routine_data_read(ADC0);
	
// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
	float vol=adc*3.3/4095;
	
// 将电压值转换为温度值  ((1.45 - 电压) * 1000 / 4.1) + 25
	float temp=((1.45 - vol) * 1000 / 4.1) + 25;
	
	printf("adc=%d   vol=%.1f   temp=%.1f\n",(int)adc,vol,temp);
}

void ADC_vol() {
	adc_routine_channel_config(ADC0,0,ADC_CHANNEL_14,ADC_SAMPLETIME_15);
	
// 使能 ADC 的软件触发 adc_software_trigger_enable
	adc_software_trigger_enable(ADC0,ADC_ROUTINE_CHANNEL);
	
// 等待 ADC 转换完成 ADC_FLAG_EOC
	while(RESET==adc_flag_get(ADC0,ADC_FLAG_EOC));

// 清除 ADC 转换完成标志位
	adc_flag_clear(ADC0,ADC_FLAG_EOC);
	
// 读取 ADC 转换结果 adc_routine_data_read
	uint16_t adc=adc_routine_data_read(ADC0);
	
// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
	float vol=adc*3.3/4095;
	
	printf("adc=%d   vol=%.2f \n",(int)adc,vol);
}
#endif

//=================================================多通道DMA===============================
#if ADC_MODE_DMA


#define ADC_RECV_LEN  2
// 用于接收两个通道的数值
uint16_t g_recv_buff[ADC_RECV_LEN];

#define DMA_CH  DMA1, DMA_CH0
#define DMA_PERIPH_ADDR   (uint32_t)(&ADC_RDATA(ADC0))
#define DMA_SUB_PERIPH    DMA_SUBPERI0

void DMA_ADC_config() {
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

void ADC_init() {
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
    adc_channel_length_config(ADC0,ADC_ROUTINE_CHANNEL,2);

    // 设置转换哪一个通道以及所处序列位置 adc_routine_channel_config
// 温度传感器需要更长的采样时间，因为内部信号源阻抗较高，需要更多时间充电采样电容
// 确保信号在采样期间能稳定到足够精度 >= ADC_SAMPLETIME_84
    adc_routine_channel_config(ADC0,0,ADC_CHANNEL_16,ADC_SAMPLETIME_84);
//    // 使能16到18内部通道(温度和参考电压通道) adc_channel_16_to_18
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);


    adc_routine_channel_config(ADC0,1,ADC_CHANNEL_14,ADC_SAMPLETIME_15);

    // 使能ADC adc_enable
    adc_enable(ADC0);
    // 内部校准(需要delay等待)
    delay_1ms(1);
    // 校准 adc_calibration_enable
    adc_calibration_enable(ADC0);

    adc_dma_request_after_last_enable(ADC0);

    adc_dma_mode_enable(ADC0);

    DMA_ADC_config();

    // 告诉ADC需要转换数据(使能触发)
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}



void ADC_get() {

// 使能 ADC 的软件触发 adc_software_trigger_enable
    adc_software_trigger_enable(ADC0,ADC_ROUTINE_CHANNEL);

// 等待 ADC 转换完成 ADC_FLAG_EOC
    while(RESET==adc_flag_get(ADC0,ADC_FLAG_EOC));

// 清除 ADC 转换完成标志位
    adc_flag_clear(ADC0,ADC_FLAG_EOC);

    //=============================================温度=========================

// 读取 ADC 转换结果 adc_routine_data_read
    uint16_t adc1=g_recv_buff[0];

// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
    float vol1=adc1*3.3/4095;

// 将电压值转换为温度值  ((1.45 - 电压) * 1000 / 4.1) + 25
    float temp=((1.45 - vol1) * 1000 / 4.1) + 25;

    printf("adc=%d   vol=%.2f   temp=%.1f\n",(int)adc1,vol1,temp);

//============================================电位器=========================
// 读取 ADC 转换结果 adc_routine_data_read
    uint16_t adc2=g_recv_buff[1];

// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
    float vol2=adc2*3.3/4095;

    printf("adc=%d   vol=%.2f \n",(int)adc2,vol2);

}

#endif
//==========================================注入模式初始化===============================================
void ADC_init() {
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
    // 设置单次模式(禁用连续转换) adc_special_function_config
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,DISABLE);
    // 设置扫描模式 adc_special_function_config
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);

    // ===================通道配置（如果加其他的在这里添加）==================================================
    // 设置是否打开插入通道(不打开) adc_special_function_config
    adc_special_function_config(ADC0,ADC_INSERTED_CHANNEL_AUTO,ENABLE);
    // 设置转换通道个数(包括常规通道组和插入通道组) adc_channel_length_config
    adc_channel_length_config(ADC0,ADC_INSERTED_CHANNEL,2);
// 确保信号在采样期间能稳定到足够精度 >= ADC_SAMPLETIME_84
    adc_inserted_channel_config(ADC0,0,ADC_CHANNEL_16,ADC_SAMPLETIME_84);
//    // 使能16到18内部通道(温度和参考电压通道) adc_channel_16_to_18
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);
		
    adc_inserted_channel_config(ADC0,1,ADC_CHANNEL_14,ADC_SAMPLETIME_15);
		
		//=====================================================================================================

    // 使能ADC adc_enable
    adc_enable(ADC0);
    // 内部校准(需要delay等待)
    delay_1ms(1);
    // 校准 adc_calibration_enable
    adc_calibration_enable(ADC0);
}


#if ADC_MODE_INJECTED
void ADC_get() {
// 使能 ADC 的软件触发 adc_software_trigger_enable
    adc_software_trigger_enable(ADC0,ADC_INSERTED_CHANNEL);

// 等待 ADC 转换完成 ADC_FLAG_EOC
    while(RESET==adc_flag_get(ADC0,ADC_FLAG_EOIC));

// 清除 ADC 转换完成标志位
    adc_flag_clear(ADC0,ADC_FLAG_EOIC);

    //=============================================温度=========================

// 读取 ADC 转换结果 adc_routine_data_read
    uint16_t adc1=adc_inserted_data_read(ADC0,ADC_INSERTED_CHANNEL_0);

// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
    float vol1=adc1*3.3/4095;

// 将电压值转换为温度值  ((1.45 - 电压) * 1000 / 4.1) + 25
    float temp=((1.45 - vol1) * 1000 / 4.1) + 25;

    printf("adc=%d   vol=%.2f   temp=%.1f\n",(int)adc1,vol1,temp);

//============================================电位器=========================
// 读取 ADC 转换结果 adc_routine_data_read
    uint16_t adc2=adc_inserted_data_read(ADC0,ADC_INSERTED_CHANNEL_1);

// 将 ADC 数值转换为电压值 采样值 * 3.3 / 4095;
    float vol2=adc2*3.3/4095;

    printf("adc=%d   vol=%.2f \n",(int)adc2,vol2);

}
#endif
