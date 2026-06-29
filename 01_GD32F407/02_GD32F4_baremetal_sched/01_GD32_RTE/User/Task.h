#ifndef __TASK_H_
#define __TASK_H_
#include "gd32f4xx.h"
#include "TIMER.h"
#include "App.h"

extern uint8_t LED188_flag;

typedef struct Task{
	uint64_t cnt;      //任务计数
	uint64_t period;   //任务周期
	void (*exec)();    //回调函数
}Task_t;

void Task_exec_handler();

#endif