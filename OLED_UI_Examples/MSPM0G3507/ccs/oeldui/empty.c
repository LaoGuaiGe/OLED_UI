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
	char buff[200]={0};
	char uart_output_buff[100]={0};
	//图形化外设配置初始化
	SYSCFG_DL_init();
	
	//串口初始化 9600
	debug_uart_init();

	//按键初始化
	user_keyBSP_init();
	
	//陀螺仪初始化
	lsm6ds3_init();
	
	// //读取W25Q128的ID
    // sprintf(uart_output_buff,"ID = %X\r\n",W25Q128_readID());
    // debug_uart_send_string(uart_output_buff);

	// //读取0地址的5个字节数据到buff
    // W25Q128_read((uint8_t*)buff, 0, 5);
    // //串口输出读取的数据
    // sprintf(uart_output_buff, "1 buff = %s\r\n",buff);
    // debug_uart_send_string(uart_output_buff);

    // //往0地址写入5个字节长度的数据 lckfb
    // W25Q128_write((uint8_t*)"lckfb", 0, 5); 
	// delay_ms(500);
    // //读取0地址的5个字节数据到buff
    // W25Q128_read((uint8_t*)buff, 0, 5);
    // //串口输出读取的数据
    // sprintf(uart_output_buff, "2 buff = %s\r\n", buff);
    // debug_uart_send_string(uart_output_buff);

	//5ms定时器中断
	timer_init();
	
	//蜂鸣器初始化
	Beeper_Init();						
	//蜂鸣器音乐  
	Beeper_Perform(BEEP1);		 
 
	//彩灯初始化
	PWM_WS2812B_Init();
	PWM_WS2812B_Red(4);  
	WS2812B_Show();



	OLED_UI_Init(&MainMenuPage);   
	
	while (1) 
	{
		// 陀螺仪测试
		// lsm6ds3_get_angle(&angle_new);
		// sprintf(buff,"angle:%.2f,%.2f,%.2f\r\n", angle_new.x,angle_new.y,angle_new.z);
		// debug_uart_send_string(buff);
		OLED_UI_MainLoop();
	}
}

