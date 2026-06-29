#include "App.h"

static uint8_t cnt=0;

uint8_t LED188_flag=0;

void App_Debug_task() {

}

void App_Debug_LED188_test_task() {

		LED188_display();
}



void App_LED188_1_199_task() {
	if(LED188_flag==0) return;
 LED188_set_num(cnt);
 cnt++;
 if(cnt>199) cnt=0;
	  
	
}