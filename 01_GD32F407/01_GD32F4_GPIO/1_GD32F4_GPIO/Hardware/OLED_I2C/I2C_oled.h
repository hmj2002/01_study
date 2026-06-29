#ifndef __I2C_OLED_H
#define __I2C_OLED_H 
// 在不需要编译的文件顶部添加
#ifndef USE_I2C_OLED
// 原有代码


#include "gd32f4xx.h"
#include "systick.h"
#include "stdlib.h"	
#include "gpio_cfg.h"
#include "I2C0_soft.h"
#include "I2C0_hard.h"
#define  I2C_WRITE(a,b,c,d)    I2C0_hard_write(a,b,c,d)      //I2C0_soft_write(a,b,c,d)

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif


// SCL - PB6
#define SCL_RCU   RCU_GPIOB
#define SCL_PORT  GPIOB
#define SCL_PIN   GPIO_PIN_6

// SDA - PB7
#define SDA_RCU   RCU_GPIOB
#define SDA_PORT  GPIOB
#define SDA_PIN   GPIO_PIN_7
//-----------------OLED端口定义---------------- 

#define OLED_SCL_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_6)//SCL
#define OLED_SCL_Set() gpio_bit_set(GPIOB,GPIO_PIN_6)

#define OLED_SDA_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_7)//DIN
#define OLED_SDA_Set() gpio_bit_set(GPIOB,GPIO_PIN_7)



#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);
#else
// 空，或者 #error 提示
#endif
#endif

