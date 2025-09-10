#ifndef _APP_KEY_TASK_H_
#define _APP_KEY_TASK_H_

#include "ti_msp_dl_config.h"
#include "mid_button.h"

typedef struct {
    unsigned int enter : 1; // 使用位字段来节省空间
    unsigned int back  : 1;
    unsigned int up    : 1;
    unsigned int down  : 1;
} KEY_MENU_STATUS;

#define PRESS 	 0		//按下
#define RELEASE  1		//松开

extern KEY_MENU_STATUS key_menu;

void btn_enter_cb(flex_button_t *btn);
void btn_back_cb(flex_button_t *btn);


#endif
