#include "bsp_DHT11.h"

#define DHT11_STATE() gpio_input_bit_get(DHT) // 读取DHT11
#define DHT_SET(bit)  gpio_bit_write(DHT, (bit) ? SET : RESET) //给DHT引脚设置电平

static void GPIO_config()
{
GPIO_output(RCU_GPIOB,GPIOB,GPIO_PIN_15, GPIO_OTYPE_OD,2);
}



//static void Delay1us(void)	//@24.000MHz
//{
//#if 0
//	for(uint8_t i=10;i<8,i++){
//  __NOP();  //使用small时候
//}
//#else
//	for(uint8_t i=10;i<5;i++){   //使用large  大内存时
//  __NOP(); 
//}
//#endif
//}


void DHT11_init() {
    GPIO_config();
    USART0_init(); 
}


char DHT11_get_humidity_temperature(int *humidity, float *temperature)
{
    uint16_t cnt;  // 是uint16_t，不是uint8_t
    uint8_t dat[5];
    uint8_t i,j;
    float temp;
	
    DHT_SET(0);
	
    delay_1ms(20);
	
    DHT_SET(1);
    // 2. 主机释放总时间 10us ~ 35us
    cnt = 0;
    do {
        cnt++;
        delay_1us(1);
    } while(DHT11_STATE()== SET && cnt < 50);
    // 只有第一次需要加50判断，其它不用
    // 50随便写的，只要>35即可，限制循环最多50次，防止DHT11损坏，导致死循环
    if (cnt < 10 || cnt > 35) {
        printf("%s: [%d]\n", "主机释放出错", (int)cnt);
        return -1; // 提前结束
    }
    // printf("cnt=%d",(int)cnt);


    cnt = 0;
    do {
        cnt++;
        delay_1us(1);
    } while(DHT11_STATE()== RESET);
    // 只有第一次需要加50判断，其它不用
    // 50随便写的，只要>35即可，限制循环最多50次，防止DHT11损坏，导致死循环
    if (cnt < 78 || cnt > 88) {
        printf("%s: [%d]\n", "响应低电平出错", (int)cnt);
        return -2; // 提前结束
    }



    cnt = 0;
    do {
        cnt++;
        delay_1us(1);
    } while(DHT11_STATE()== SET);
    // 只有第一次需要加50判断，其它不用
    // 50随便写的，只要>35即可，限制循环最多50次，防止DHT11损坏，导致死循环
    if (cnt < 80 || cnt > 92) {
        printf("%s: [%d]\n", "响应高电平出错", (int)cnt);
        return -3; // 提前结束
    }


    for(i=0; i<5; i++) {
        for(j=0; j<8; j++) {
            cnt = 0;
            do {
                cnt++;
                delay_1us(1);
            } while(DHT11_STATE()== RESET);

            if (cnt < 50 || cnt > 58) {
                printf("%s: [%d]\n", "信号低电平出错", (int)cnt);
                return -4; // 提前结束
            }

            cnt = 0;
            do {
                cnt++;
                delay_1us(1);
            } while(DHT11_STATE()== SET);
            dat[i]<<=1;
            if (cnt >= 68) {
                dat[i]|=1;
            }
        }
    }


    if(dat[0]+dat[1]+dat[2]+dat[3]!=dat[4]) {

        printf("校验失败\n");
        return -5;
    }
    printf("校验成功\n");

    temp=dat[2]+(dat[3]&0x7f)*0.1;
    if(dat[3]&0x80) {
        temp=-temp;
    }



    *humidity=dat[0];
    *temperature= temp;
    return 0;
}