/*
 *  OLED_UI_Launcher.c
 *  Version: 0.2.1
 *  Created on: Jun 26, 2025
 *  Author: 5akura1
 */
#include "OLED_UI_Launcher.h"

void OLED_UI_init(void)
{
	// init
	// ������Χ����
	// ע�⣺������Χ�������ܵ��µ�ѹ��������Ҫ�ڴ˲���֮��������дFlash
	//	HAL_GPIO_WritePin(PWRON_GPIO_Port, PWRON_Pin, 0);
	//	BTN_init();
	OLED_UI_Init(&MainMenuPage);
}

void OLED_UI_start(void)
{
	while (1)
	{
		//		BtnTask();

		OLED_UI_MainLoop();
	}
}

// ��ʱ��1�жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim1)
	{
		// �жϺ���
		OLED_UI_InterruptHandler();
	}
}
