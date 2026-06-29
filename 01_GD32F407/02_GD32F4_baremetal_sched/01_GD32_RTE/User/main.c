#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "USART0.h"
#include "TIMER.h"
#include "Task.h"

uint8_t enable_charge=0;
uint8_t enable_percent=0;

void USART0_on_recv(uint8_t* data, uint32_t len) {
   printf("recv[%d] = %s\n", len, data);
	if(data[0]==0x01){
	enable_charge=!enable_charge;
	LED188_charge(enable_charge); 
  
		
	}else if(data[0]==0x02){
	enable_percent=!enable_percent;
  LED188_percent(enable_percent);
	
	}
	else if(data[0]==0x03){
		
	LED188_flag=0;
		
  LED188_stop();
	
	}
	else if(data[0]==0x04){
	
  LED188_flag=!LED188_flag;
	}
	
	
}

int main(void) {
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 系统滴答定时器初始化
    systick_config();


    USART0_init();
    TIMER_init();
    App_init();
    while(1) {
		Task_exec_handler();
    }
}



