#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include "gd32f4xx.h"
#include "gpio_cfg.h"
#if USE_SPI0_soft_hard

#include "SPI0_hard.h"
#define  spi_read_write_byte(dat)        SPI0_hard_read_write(dat)
#define  SPI0_init()                     SPI0_hard_init()
#else
#include "SPI0_soft.h"
#define  spi_read_write_byte(dat)        SPI0_soft_read_write(dat)
#define  SPI0_init()                     SPI0_soft_init()

#endif

#define BSP_GPIO_RCU			RCU_GPIOA
#define BSP_SPI_RCU				RCU_SPI0
#define BSP_SPI_NSS_RCU			RCU_GPIOA

#define BSP_GPIO_PORT			GPIOA
#define BSP_GPIO_AF				GPIO_AF_5

#define BSP_SPI					SPI0
#define BSP_SPI_NSS				GPIO_PIN_4
//#define BSP_SPI_SCK				GPIO_PIN_5
//#define BSP_SPI_MISO			GPIO_PIN_6
//#define BSP_SPI_MOSI			GPIO_PIN_7

#define W25QXX_CS_ON(x)		(gpio_bit_write(BSP_GPIO_PORT, BSP_SPI_NSS, x?1:0))

void bsp_spi_init(void);
uint8_t spi_read_write_byte(uint8_t dat);
uint16_t GD25Q32_readID(void);
void GD25Q32_write_enable(void);
void GD25Q32_wait_busy(void);
void GD25Q32_erase_sector(uint32_t addr);
void GD25Q32_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void GD25Q32_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length);

/*//===================================使用示例=========================================
  unsigned char buff[20] = {0};
    bsp_spi_init(); // SPI flash
    // 获取GD25Q32的设备ID
    printf("ID = %X\r\n",GD25Q32_readID());
    //读取0地址长度为11个字节的数据到buff
    GD25Q32_read(buff, 0, 11);
    //输出读取到的数据
    printf("22222 buff: %s\r\n",buff);
    //		delay_ms(200);
    //往0地址写入11个字节的数据
    GD25Q32_write((uint8_t *)"黑马程序员", 0, 11);

    // 等待写入完成
    //		delay_ms(200);
    //读取0地址长度为11个字节的数据到buff
    GD25Q32_read(buff, 0, 11);
    //输出读取到的数据
    printf("33333 buff: %s\r\n",buff);
*/
#endif