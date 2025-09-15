#ifndef _BSP_W25QXX_H__
#define _BSP_W25QXX_H__

#include "ti_msp_dl_config.h"

//CS引脚的输出控制
//x=0时输出低电平
//x=1时输出高电平
#define SPI_CS(x)  ( (x) ? DL_GPIO_setPins(FLASH_PORT,FLASH_CS_PIN) : DL_GPIO_clearPins(FLASH_PORT,FLASH_CS_PIN) )

uint16_t W25Q128_readID(void);//读取W25Q128的ID
void W25Q128_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);      //W25Q128写数据
void W25Q128_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length);//W25Q128读数据
#endif