#include "bsp_ec12.h"

void EC12_init() {
    printf("EC12_init\n");
    GPIO_input(EC12_B_RCU,EC12_B_PIN,GPIO_PUPD_NONE);
}


void EC12_A_ON_TRIG() {

    static FlagStatus last_a_state=SET;
    FlagStatus cur_a_state = EC12_A_STATE();
    if(cur_a_state!=last_a_state) {
        FlagStatus b_state = EC12_B_STATE();
        if (cur_a_state == RESET) {// 下降沿
            if(b_state==SET) {

#if EC12_USE_RIGHT
                EC12_on_turn_right();
#endif
            } else {
#if EC12_USE_LEFT
                EC12_on_turn_left();
#endif
            }
					
        } else { // 上升沿
            if(b_state==RESET) {
#if EC12_USE_RIGHT
                EC12_on_turn_right();
#endif
            } else {
#if EC12_USE_LEFT
                EC12_on_turn_left();
#endif
            }
        }
    }
    last_a_state = cur_a_state; // 保存状态
}


void EC12_D_ON_TRIG() {

    static FlagStatus last_state=SET;
    FlagStatus cur_state = EC12_D_STATE();

    if (last_state == SET && cur_state == RESET) {// 上一次是抬起，当前是按下，按下才有效
        last_state = RESET; // 保存状态
#if EC12_USE_DOWN
        EC12_on_down();
#endif


    } else if (last_state == RESET && cur_state == SET) {// 上一次是按下，当前是抬起，抬起才有效
        last_state = SET; // 保存状态
#if EC12_USE_UP
        EC12_on_up();
#endif
    }
}

__attribute__((weak)) void EC12_on_down() {
    printf("EC12_on_down\n");
}

__attribute__((weak)) void EC12_on_up() {
    printf("EC12_on_up\n");
}

__attribute__((weak)) void EC12_on_turn_right() { // 顺时针，在时钟的上半部分，指针是向右移动的
    printf("顺时针\n");
}
__attribute__((weak)) void EC12_on_turn_left() {
    printf("逆时针\n");
}