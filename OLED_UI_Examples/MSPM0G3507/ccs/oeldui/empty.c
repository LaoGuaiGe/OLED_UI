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

#include "app_game.h"

extern bool ColorMode;
extern bool OLED_UI_ShowFps;
extern BEEPER_Tag Beeper0;
extern int16_t OLED_UI_Brightness;


int main(void)
{
	int time_num = 0;
	//图形化外设配置初始化
	SYSCFG_DL_init();
	
	//串口初始化 9600
	debug_uart_init();

	Delay_ms(500);

	//蜂鸣器初始化
	Beeper_Init();	

	//读取
	uint8_t temp[5] = {0};
	W25Q128_read(temp, 0, 5); 
	OLED_UI_Brightness 		= (uint16_t)temp[0];
	Beeper0.Sound_Loud 		= temp[1];
	Beeper0.Beeper_Enable 	= temp[2];
	ColorMode 				= (bool)temp[3];
	OLED_UI_ShowFps 		= (bool)temp[4];

	//按键初始化
	user_keyBSP_init();

	//5ms定时器中断
	timer_init();
	
	//蜂鸣器音乐  
	Beeper_Perform(BEEP1);		 

	// //UI初始化
	OLED_UI_Init(&MainMenuPage);   

	game_init();
	while (1) 
	{
		//OLED_UI_MainLoop();
		
		game_loop();
	}
}

