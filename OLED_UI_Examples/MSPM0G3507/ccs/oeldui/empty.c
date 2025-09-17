#include "ti_msp_dl_config.h"
#include "string.h"
#include "stdio.h"

#include "mid_timer.h"

#include "mid_debug_uart.h"

#include "mid_button.h"

#include "OLED_UI.h"
#include "OLED_UI_MenuData.h"

#include "mid_music.h"

#include "hw_delay.h"

#include "hw_lsm6ds3.h"

#include "hw_ws2812.h"

#include "hw_w25qxx.h"



int main(void)
{

	//图形化外设配置初始化
	SYSCFG_DL_init();
	
	//串口初始化 9600
	debug_uart_init();

	Delay_ms(500);
	//按键初始化
	user_keyBSP_init();

	//5ms定时器中断
	timer_init();
	
	//蜂鸣器初始化
	Beeper_Init();						
	//蜂鸣器音乐  
	Beeper_Perform(BEEP1);		 

	OLED_UI_Init(&MainMenuPage);   

	while (1) 
	{
		OLED_UI_MainLoop();
	}
}

