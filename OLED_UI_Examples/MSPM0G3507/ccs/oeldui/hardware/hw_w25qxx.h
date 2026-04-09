#ifndef _BSP_W25QXX_H__
#define _BSP_W25QXX_H__

#include "ti_msp_dl_config.h"

//CS引脚的输出控制
//x=0时输出低电平
//x=1时输出高电平
#define SPI_CS(x)  ( (x) ? DL_GPIO_setPins(FLASH_PORT,FLASH_CS_PIN) : DL_GPIO_clearPins(FLASH_PORT,FLASH_CS_PIN) )

// 系统参数存储配置
#define SETTINGS_SECTOR_ADDR   0xFFF000   // 使用最后一个扇区，避免与字体等数据冲突
#define SETTINGS_DATA_SIZE     5          // 参数数据字节数
#define SETTINGS_RECORD_SIZE   6          // 每条记录大小（1字节标记 + 5字节数据）
#define SETTINGS_MAGIC         0xAA       // 有效记录标记

uint16_t W25Q128_readID(void);
void W25Q128_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void W25Q128_read(uint8_t* buffer, uint32_t read_addr, uint16_t read_length);
void W25Q128_test(void);

// wear leveling 接口
void settings_save(uint8_t* data);
void settings_load(uint8_t* data);

#endif