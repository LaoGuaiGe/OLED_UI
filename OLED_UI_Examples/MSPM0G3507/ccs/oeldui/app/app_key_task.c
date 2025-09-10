#include "app_key_task.h"


KEY_MENU_STATUS key_menu={1,1,1,1};
char status_enter=0;
char status_back=0;
//进 右
void btn_enter_cb(flex_button_t *btn)
{
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_CLICK: //单击事件 

            key_menu.down = PRESS;
           

		case FLEX_BTN_PRESS_LONG_START://长按开始事件 	 
            key_menu.enter = PRESS;
			//key_menu.down = RELEASE;
			break;			

        default:
			// key_menu.enter = RELEASE;
			// key_menu.down = RELEASE;
			break;
    }
}

//退 左
void btn_back_cb(flex_button_t *btn)
{
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_CLICK: //单击事件
            key_menu.up = PRESS;
            break;
       case FLEX_BTN_PRESS_LONG_START://长按开始事件	 
			key_menu.back = PRESS;
           break;

        default:
			// key_menu.up = RELEASE;
			// key_menu.back = RELEASE;
			break;
    }
}
