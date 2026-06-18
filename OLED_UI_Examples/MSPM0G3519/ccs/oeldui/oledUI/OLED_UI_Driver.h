#ifndef __OLED_UI_DRIVER_H
#define __OLED_UI_DRIVER_H
/*【如果您需要移植此项目，则需要更改以下函数的实现方式。】 */
#include <stdint.h>

// 按键状态读取（按下0 松开1）。实现见 OLED_UI_Driver.c，移植时只改该 .c。
uint8_t Key_GetEnterStatus(void);
uint8_t Key_GetBackStatus(void);
uint8_t Key_GetUpStatus(void);
uint8_t Key_GetDownStatus(void);

//定时器中断初始化函数
void Timer_Init(void);

//按键初始化函数
void Key_Init(void);

//编码器初始化函数
void Encoder_Init(void);

// 编码器使能函数
void Encoder_Enable(void);

//编码器失能函数
void Encoder_Disable(void);

//读取编码器的增量值
int16_t Encoder_Get(void);

//延时函数
void Delay_us(uint32_t xus);
void Delay_ms(uint32_t xms);
void Delay_s(uint32_t xs);

#endif
