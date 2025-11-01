#include "mid_timer.h"
#include "mid_button.h"
#include "mid_debug_led.h"
#include "OLED_UI.h"
#include "OLED_UI_MenuData.h"
#include "mid_music.h"

// 定义枚举类型
typedef enum {
    TASK_DISABLE=0, 
    TASK_ENABLE   
} Task_state;

static volatile Task_state tack_enable_flag = TASK_ENABLE;

//使能任务调度
void enable_task_interrupt(void)
{
	tack_enable_flag = TASK_ENABLE;
}
//失能任务调度
void disable_task_interrupt(void)
{
	tack_enable_flag = TASK_DISABLE;
}
//获取任务状态
Task_state get_task_status(void)
{
	return tack_enable_flag;
}

void timer_init(void)
{
    //定时器中断
	NVIC_ClearPendingIRQ(TIMER_TICK_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_TICK_INST_INT_IRQN);
}

//5ms定时器中断服务函数
void TIMER_TICK_INST_IRQHandler(void)
{
	static char oled_time_num = 0, buzzer_time_num = 0;
	//5ms归零中断触发
	if( DL_TimerA_getPendingInterrupt(TIMER_TICK_INST) == DL_TIMER_IIDX_ZERO )
	{

		//按键扫描
		button_ticks();
		
		// 屏幕刷新 4x5ms=20ms
		if( (oled_time_num++) >= 4 )
		{
			oled_time_num = 0;
			OLED_UI_InterruptHandler();
			set_debug_led_toggle();
		}

		// 蜂鸣器音调控制 2x5ms=10ms
		if( (buzzer_time_num++) >= 1 )
		{
			buzzer_time_num = 0;
			Beeper_Proc();
		}
	}
}