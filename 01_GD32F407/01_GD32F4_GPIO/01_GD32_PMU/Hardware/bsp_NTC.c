#include "bsp_NTC.h"

static void GPIO_config() { 
GPIO_analog(RCU_GPIOA,GPIOA,GPIO_PIN_1);
}
/******************* ADC配置函数 *******************/


#define ADC_RECV_LEN  1
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

void ADC_config(){
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


    adc_routine_channel_config(ADC0,0,ADC_CHANNEL_1,ADC_SAMPLETIME_15);

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


uint16_t  temp_table[] = { 
    //阻值    温度
	58354, // -55
	55464, // -54
	52698, // -53
	50048, // -52
	47515, // -51
	45097, // -50
	42789, // -49
	40589, // -48
	38492, // -47
	36496, // -46
	34597, // -45
	32791, // -44
	31075, // -43
	29444, // -42
	27896, // -41
	26427, // -40
	25034, // -39
	23713, // -38
	22460, // -37
	21273, // -36
	20148, // -35
	19083, // -34
	18075, // -33
	17120, // -32
	16216, // -31
	15361, // -30
	14551, // -29
	13785, // -28
	13061, // -27
	12376, // -26
	11728, // -25
	11114, // -24
	10535, // -23
	9986,  // -22
	9468,  // -21
	8977,  // -20
	8513,  // -19
	8075,  // -18
	7660,  // -17
	7267,  // -16
	6896,  // -15
	6545,  // -14
	6212,  // -13
	5898,  // -12
	5601,  // -11
	5319,  // -10
	5053,  // -9
	4801,  // -8
	4562,  // -7
	4336,  // -6
	4122,  // -5
	3920,  // -4
	3728,  // -3
	3546,  // -2
	3374,  // -1
	3211,  // 0
	3057,  // 1
	2910,  // 2
	2771,  // 3
	2639,  // 4
	2515,  // 5
	2396,  // 6
	2284,  // 7
	2177,  // 8
	2076,  // 9
	1978,  // 10
	1889,  // 11
	1802,  // 12
	1720,  // 13
	1642,  // 14
	1568,  // 15
	1497,  // 16
	1430,  // 17
	1366,  // 18
	1306,  // 19
	1248,  // 20
	1193,  // 21
	1141,  // 22
	1092,  // 23
	1044,  // 24
	1000,  // 25
	957,   // 26
	916,   // 27
	877,   // 28
	840,   // 29
	805,   // 30
	771,   // 31
	739,   // 32
	709,   // 33
	679,   // 34
	652,   // 35
	625,   // 36
	600,   // 37
	576,   // 38
	552,   // 39
	530,   // 40
	509,   // 41
	489,   // 42
	470,   // 43
	452,   // 44
	434,   // 45
	417,   // 46
	401,   // 47
	386,   // 48
	371,   // 49
	358,   // 50
	344,   // 51
	331,   // 52
	318,   // 53
	306,   // 54
	295,   // 55
	284,   // 56
	274,   // 57
	264,   // 58
	254,   // 59
	245,   // 60
	236,   // 61
	228,   // 62
	220,   // 63
	212,   // 64
	205,   // 65
	198,   // 66
	191,   // 67
	184,   // 68
	178,   // 69
	172,   // 70
	166,   // 71
	160,   // 72
	155,   // 73
	150,   // 74
	145,   // 75
	140,   // 76
	135,   // 77
	131,   // 78
	126,   // 79
	122,   // 80
	118,   // 81
	115,   // 82
	111,   // 83
	107,   // 84
	104,   // 85
	101,   // 86
	97,    // 87
	94,    // 88
	91,    // 89
	89,    // 90
	86,    // 91
	83,    // 92
	81,    // 93
	78,    // 94
	76,    // 95
	74,    // 96
	71,    // 97
	69,    // 98
	67,    // 99
	65,    // 100
	63,    // 101
	61,    // 102
	60,    // 103
	58,    // 104
	56,    // 105
	55,    // 106
	53,    // 107
	52,    // 108
	50,    // 109
	49,    // 110
	47,    // 111
	46,    // 112
	45,    // 113
	43,    // 114
	42,    // 115
	41,    // 116
	40,    // 117
	39,    // 118
	38,    // 119
	37,    // 120
	36,    // 121
	35,    // 122
	34,    // 123
	33,    // 124
	32,    // 125
};

#define abs(x)	((x > 0) ? (x) : (-(x))) // 宏定义函数，求绝对值
// 函数功能：在temp_table 的数组中查找最接近 rst_Rx10 的元素，并返回该元素的索引
static int search_temp(float rst_Rx10){
	int i, min_index = 0;
	float min_diff;
	
	// 计算数组长度
	int len = sizeof(temp_table) / sizeof(u16);
	// 放大100倍
    rst_Rx10 = rst_Rx10 * 100; 
	// 记录最小差值 
	min_diff = abs(rst_Rx10 - temp_table[0]);

	for (i = 1; i < len; i++){
		// 计算数组里每一个阻值和rst_Rx10的差值
		float diff = abs(rst_Rx10 - temp_table[i]);
		
		// 得到差值最小元素对应的索引i
		if(diff < min_diff){
			// 如果有更小的差值，赋值
			min_diff = diff;
			min_index = i;
		}
	}
	
	return min_index;
}


void NTC_init() {  // 初始化
	// IO 初始化
	GPIO_config();
	// ADC配置
	ADC_config();
}
int NTC_get_temperature() { // 获取温度

	// adc 读取内容 
	u16 adc = g_recv_buff[0];
	// adc 转换为 电压
	float vol = adc * 3.3 / 4095;
	// 转换为电阻
	float r = vol * 10 / (3.3 - vol);
	// 电阻 找到 差值最小的下标    -55 就是温度
  int  temp = search_temp(r) - 55;
	
	return temp;
}