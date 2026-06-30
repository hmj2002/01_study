#include "App.h"

static int cnt=100;//数码管显示数字
static uint8_t ADkey_flag=0;//运行和暂停标志位  0：
static uint8_t ADkey_add_sub_flag=0;

void App_ADKey_task() {
    ADKey_scan();
}

//188数码管++任务
void App_LED188_1_199_add_task() {
    if(ADkey_flag==0) return;
    if(ADkey_add_sub_flag==1) return;
    cnt++;
    if(cnt>199) cnt=0;
    LED188_set_num(cnt);
}

//188数码管--任务
void App_LED188_199_1_sub_task() {
    if(ADkey_flag==0) return;
    if(ADkey_add_sub_flag==0) return;
    cnt--;
    if(cnt<0) cnt=199;
    LED188_set_num(cnt);


}

//按键一长按按键运行和暂停
void ADCKey_on_long_click(uint8_t i)
{
    if(i==1) {
        ADkey_flag=!ADkey_flag;
    }
}

//按键三短按切换+和-任务
void ADCKey_on_click(uint8_t i) {
    if(i==3) {
        ADkey_add_sub_flag=!ADkey_add_sub_flag;
    }
}