#ifndef __OLED_UI_DRIVER_H
#define __OLED_UI_DRIVER_H
/*���������Ҫ��ֲ����Ŀ������Ҫ�������º�����ʵ�ַ�ʽ���� */
#include "stm32f4xx_hal.h" // Device header
#include "stm32f4xx.h"
#include "OLED_UI\misc.h"

// ��ȡȷ�ϣ�ȡ�����ϣ��°���״̬�ĺ���(��Q��Ϊʲôʹ�ú궨������Ǻ�����A����Ϊ�����������Ч�ʣ����ٴ�������)
// #define Key_GetEnterStatus()	(!BTN_stat.isDebouncedPressing)
// #define Key_GetBackStatus()		(1)
// #define Key_GetUpStatus()		(1)
// #define Key_GetDownStatus()		(1)

#define Key_GetEnterStatus()    HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define Key_GetBackStatus()     HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)
#define Key_GetUpStatus()       HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define Key_GetDownStatus()     HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)

// ��ʱ���жϳ�ʼ������
void Timer_Init(void);

// ������ʼ������
void Key_Init(void);

//��������ʼ������
void Encoder_Init(void);
// ������ʹ�ܺ���
void Encoder_Enable(void);
//������ʧ�ܺ���
void Encoder_Disable(void);
//��ȡ������������ֵ
int16_t Encoder_Get(void);

// ��ʱ����
// void Delay_us(uint32_t xus);
void Delay_ms(uint32_t xms);
void Delay_s(uint32_t xs);

#endif
