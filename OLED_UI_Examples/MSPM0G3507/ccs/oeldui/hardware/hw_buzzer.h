#ifndef _HW_BUZZER_H_
#define _HW_BUZZER_H_

#include "ti_msp_dl_config.h"

void buzzer_on(void);
void buzzer_off(void);
void buzzer_Set_duty(uint16_t value);
void buzzer_Set_reload_value(uint16_t value);

#endif

