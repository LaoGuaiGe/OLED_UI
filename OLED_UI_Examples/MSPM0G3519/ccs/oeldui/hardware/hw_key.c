#include "hw_key.h"

KEY_STATUS key_scan(void)
{
    KEY_STATUS states;

    // PA18 高电平按下 OR PA24 低电平按下 → back=0 表示按下(button库active_level=0)
    uint32_t pa18 = DL_GPIO_readPins(KEY_BACK_PORT, KEY_BACK_PIN);
    uint32_t pa24 = DL_GPIO_readPins(KEY_A24_PORT, KEY_A24_PIN);
    states.back = (pa18 || !pa24) ? 0 : 1;

    // PB21 低电平按下 OR PB24 低电平按下 → enter=0 表示按下
    uint32_t pb21 = DL_GPIO_readPins(KEY_ENTER_PORT, KEY_ENTER_PIN);
    uint32_t pb24 = DL_GPIO_readPins(KEY_B24_PORT, KEY_B24_PIN);
    states.enter = (!pb21 || !pb24) ? 0 : 1;

    states.encoder_sw = DL_GPIO_readPins(KEY_ENCODER_SW_PORT, KEY_ENCODER_SW_PIN) ? 1 : 0;

    return states;
}