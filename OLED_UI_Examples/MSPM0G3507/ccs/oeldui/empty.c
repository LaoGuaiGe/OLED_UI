#include "ti_msp_dl_config.h"
#include "string.h"
#include "stdio.h"

#include "mid_timer.h"

#include "mid_debug_uart.h"

#include "mid_button.h"

#include "OLED_UI.h"
#include "OLED_UI_MenuData.h"



int main(void)
{
	SYSCFG_DL_init();

	debug_uart_init();

	user_button_init();
	
	timer_init();
	
	OLED_UI_Init(&MainMenuPage);

	
	while (1) 
	{
		OLED_UI_MainLoop();

	}
}

