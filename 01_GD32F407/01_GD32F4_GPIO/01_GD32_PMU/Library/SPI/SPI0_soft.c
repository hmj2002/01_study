#include "SPI0_soft.h"

// 初始化
void SPI0_soft_init(){
  
  GPIO_output(SPI0S_SCL_RCU,SPI0S_SCL_PORT,SPI0S_SCL_PIN,GPIO_OTYPE_PP,1);
	GPIO_output(SPI0S_MOSI_RCU,SPI0S_MOSI_PORT,SPI0S_MOSI_PIN,GPIO_OTYPE_PP,1);
	//PA6上拉输入
	GPIO_input(SPI0S_MISO_RCU,SPI0S_MISO_PORT, SPI0S_MISO_PIN,GPIO_PUPD_PULLUP);

}
// 写数据
void SPI0_soft_write(uint8_t dat){
	for(u8 i=0;i<8;i++)
	{
		SPI0S_SCL(0);
		if(dat&0x80)
		{
			SPI0S_MOSI(1);
    }
		else
		{
			SPI0S_MOSI(0);
    }
		dat<<=1;
		SPI0S_SCL(1);
  }

}
// 读数据
uint8_t SPI0_soft_read(){

	u8 i,read=0;
	for(i=0;i<8;i++)
	{
		SPI0S_SCL(0);
		read<<=1;
		if(SPI0S_MISO())
		{
			read++;
    }
		SPI0S_SCL(1);
  }
	return read;
}

//读写
uint8_t SPI0_soft_read_write(uint8_t dat){
	u8 read=0;
	for(u8 i=0;i<8;i++)
	{
		SPI0S_SCL(0);
		if(dat&0x80)
		{
			SPI0S_MOSI(1);
    }
		else
		{
			SPI0S_MOSI(0);
    }
		dat<<=1;
		
		
		read<<=1;
		if(SPI0S_MISO())
		{
			read++;
    }
		SPI0S_SCL(1);

}
		return read;
	}