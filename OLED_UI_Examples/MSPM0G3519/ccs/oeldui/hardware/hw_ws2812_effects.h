#ifndef __HW_WS2812_EFFECTS_H__
#define __HW_WS2812_EFFECTS_H__

#include "stdint.h"
#include "stdbool.h"

typedef enum {
    EFFECT_STATIC = 0,
    EFFECT_FLOWING = 1,
    EFFECT_RUNNING = 2,
    EFFECT_BREATHE = 3,
} WS2812_Effect_Mode;

typedef struct {
    WS2812_Effect_Mode mode;
    uint16_t speed;
    uint8_t brightness;
} WS2812_Effect_Param;

WS2812_Effect_Param* ws2812_effect_param(void);   // 效果参数句柄(供菜单绑定 &...->speed)
int16_t*             ws2812_light_mode_ref(void); // 灯效模式句柄 0=关 1=流水 2=跑马 3=呼吸

void ws2812_set_effect_mode(WS2812_Effect_Mode mode);
void ws2812_set_effect_speed(uint16_t speed);
void ws2812_effect_update(void);
void ws2812_effect_tick(void);

#endif
