#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include "stdio.h"
#include "main.h"

// �����ͺ�ѡ��
#define SSD1306
// #define SH1106

// �Ƿ�ʹ��Ӳ��SPI��ע�͸�����ֱ�ӿ���GPIO��
#define OLED_UI_USE_HW_SPI

// ʹ��Ӳ��SPI
#ifdef OLED_UI_USE_HW_SPI
#define OLED_UI_SPI_USE_DMA         // ʹ��DMA
#define OLED_UI_SPI_NSS_HARD_OUTPUT // Ӳ��CS��ע�͸�����ʹ���������OLED��CS pin��

#define OLED_DC_Clr() 	(HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)) // ��λ DC (��DC��������)
#define OLED_DC_Set() 	(HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)) // ��λ DC (��DC��������)

#define OLED_RES_Clr() 	(HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)) // ��λ DC (��DC��������)
#define OLED_RES_Set() 	(HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)) // ��λ DC (��DC��������)

// ��ʹ��Ӳ��SPI
#else

// ʹ�ú궨�壬�ٶȸ��죨�Ĵ�����ʽ��
#define OLED_SCL_Clr() (OLED_CLK_GPIO_Port->BRR = OLED_CLK_Pin)  // ��λ SCL (�� GPIOB �� 8 ����������)
#define OLED_SCL_Set() (OLED_CLK_GPIO_Port->BSRR = OLED_CLK_Pin) // ��λ SCL (�� GPIOB �� 8 ����������)

#define OLED_SDA_Clr() (OLED_MOSI_GPIO_Port->BRR = OLED_MOSI_Pin)  // ��λ SDA (�� GPIOB �� 9 ����������)
#define OLED_SDA_Set() (OLED_MOSI_GPIO_Port->BSRR = OLED_MOSI_Pin) // ��λ SDA (�� GPIOB �� 9 ����������)

#define OLED_RES_Clr() (OLED_RST_GPIO_Port->BRR = OLED_RST_Pin)  // ��λ RES (�� GPIOB �� 5 ����������)
#define OLED_RES_Set() (OLED_RST_GPIO_Port->BSRR = OLED_RST_Pin) // ��λ RES (�� GPIOB �� 5 ����������)

#define OLED_DC_Clr() (OLED_DC_GPIO_Port->BRR = OLED_DC_Pin)  // ��λ DC (�� GPIOB �� 6 ����������)
#define OLED_DC_Set() (OLED_DC_GPIO_Port->BSRR = OLED_DC_Pin) // ��λ DC (�� GPIOB �� 6 ����������)

#define OLED_CS_Clr() (OLED_CS_GPIO_Port->BRR = OLED_CS_Pin)  // ��λ CS (�� GPIOB �� 7 ����������)
#define OLED_CS_Set() (OLED_CS_GPIO_Port->BSRR = OLED_CS_Pin) // ��λ CS (�� GPIOB �� 7 ����������)

#endif

#define OLED_CMD 0  // д����
#define OLED_DATA 1 // д����

//	oled��ʼ������
void OLED_Init(void);
//	oledȫ��ˢ�º���
void OLED_Update(void);
//	oled�ֲ�ˢ�º���
void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);
// ������ɫģʽ
void OLED_SetColorMode(bool colormode);
// OLED �������Ⱥ���
void OLED_Brightness(int16_t Brightness);

#endif
