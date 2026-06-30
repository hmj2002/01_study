#include "Task.h"

#define TASK_LEN   sizeof(task_list)/sizeof(task_list[0])

Task_t task_list[]={
{0,2,App_Debug_LED188_test_task},
{0,500,App_LED188_1_199_task},
{0,1,App_ADKey_task},
//{0,1000,App_Voltage_task},
//{0,1000,App_Current_task},
{0,500,App_LED188_1_199_add_task},
{0,500,App_LED188_199_1_sub_task},
};


// timer5的中断回调函数，每隔1ms执行1次
volatile uint64_t g_t5_tick_ms = 0; // 全局变量定义
// Tiemr5中断函数回调函数，每隔1ms执行1次
void TIMER5_irq_callback() {
    g_t5_tick_ms++; // 累加
}


void Task_exec_handler(){
	for(uint16_t i=0;i<TASK_LEN;i++){
	if(g_t5_tick_ms-task_list[i].cnt<task_list[i].period) continue;
  task_list[i].cnt=g_t5_tick_ms;
	task_list[i].exec();
	}
}