/*
 *  OLED_UI_Launcher.h
 *  Version: 0.2.0
 *  Created on: Jun 26, 2025
 *  Author: 5akura1
 */

#ifndef __OLED_UI_LAUNCHER_H
#define __OLED_UI_LAUNCHER_H
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "main.h"
#include "OLED_UI.h"
#include "OLED_UI_MenuData.h"
#include "misc.h"
#include "OLED.h"

// 定时器句柄
extern TIM_HandleTypeDef htim1;

void OLED_UI_init();
void OLED_UI_start();

#endif /* __OLED_UI_LAUNCHER_H_ */
