#ifndef __OLED_H
#define __OLED_H 

#include "gpio_cfg.h"
#include "systick.h"


#if USE_SPI0_soft_hard

#include "SPI0_hard.h"
#define  SPI0_init()        SPI0_hard_init()
#define  SPI0_write(dat)    SPI0_hard_write(dat)  
#define  SPI0_read()        SPI0_hard_read()
#else
#include "SPI0_soft.h"
#define  SPI0_init()        SPI0_soft_init()
#define  SPI0_write(dat)    SPI0_soft_write(dat)  
#define  SPI0_read()        SPI0_soft_read()
#endif
//SCL=SCLK 
//SDA=MOSI
//DC=DC
//CS=CS1
//FS0=MOSI
//CS2=CS2


////SCL=SCLK  PA5
//#define SCL_RCU     RCU_GPIOA
//#define SCL_PORT    GPIOA
//#define SCL_PIN     GPIO_PIN_5
////SDA=MOSI	PA7
//#define MOSI_RCU    RCU_GPIOA
//#define MOSI_PORT   GPIOA
//#define MOSI_PIN    GPIO_PIN_7
//DC=DC		PA2
#define DC_RCU      RCU_GPIOA
#define DC_PORT     GPIOA
#define DC_PIN      GPIO_PIN_2
//CS=CS1  	PA3
#define CS1_RCU     RCU_GPIOA
#define CS1_PORT    GPIOA
#define CS1_PIN     GPIO_PIN_3
////FS0=MISO	PA6
//#define MISO_RCU    RCU_GPIOA
//#define MISO_PORT   GPIOA
//#define MISO_PIN    GPIO_PIN_6
//CS2=CS2 	PC5
#define CS2_RCU     RCU_GPIOC
#define CS2_PORT    GPIOC
#define CS2_PIN     GPIO_PIN_5


//-----------------OLED端口定义---------------- 

//#define OLED_SCL_Clr() gpio_bit_reset(SCL_PORT,SCL_PIN)//SCL
//#define OLED_SCL_Set() gpio_bit_set(SCL_PORT,SCL_PIN)

//#define OLED_SDA_Clr() gpio_bit_reset(MOSI_PORT,MOSI_PIN)//SDA
//#define OLED_SDA_Set() gpio_bit_set(MOSI_PORT,MOSI_PIN)

#define OLED_DC_Clr()  gpio_bit_reset(DC_PORT,DC_PIN)//DC
#define OLED_DC_Set()  gpio_bit_set(DC_PORT,DC_PIN)
 		     
#define OLED_CS_Clr()  gpio_bit_reset(CS1_PORT,CS1_PIN)//CS1
#define OLED_CS_Set()  gpio_bit_set(CS1_PORT,CS1_PIN)
 		     
//#define OLED_READ_FS0()    gpio_input_bit_get(MISO_PORT,MISO_PIN)//FS0

#define OLED_ROM_CS_Clr()  gpio_bit_reset(CS2_PORT,CS2_PIN)//CS2
#define OLED_ROM_CS_Set()  gpio_bit_set(CS2_PORT,CS2_PIN)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Clear(void);
void OLED_address(u8 x,u8 y);
void OLED_Display_128x64(u8 *dp);
void OLED_Display_16x16(u8 x,u8 y,u8 *dp);
void OLED_Display_8x16(u8 x,u8 y,u8 *dp);
void OLED_Display_5x7(u8 x,u8 y,u8 *dp);
void Send_Command_to_ROM(u8 dat);
u8 Get_data_from_ROM(void);
void OLED_get_data_from_ROM(u8 addrHigh,u8 addrMid,u8 addrLow,u8 *pbuff,u8 DataLen);
void OLED_Display_GB2312_string(u8 x,u8 y,u8 *text);
void OLED_Display_string_5x7(u8 x,u8 y,u8 *text);
void OLED_ShowNum(u8 x,u8 y,float num,u8 len);
void OLED_Init(void);
#endif

