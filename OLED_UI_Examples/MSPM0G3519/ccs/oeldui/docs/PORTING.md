# OLED_UI 工程移植说明

本工程经过解耦重构（2026-06-18），目标是让 OLED_UI 框架可独立移植、各模块通过 API 通信而非裸全局变量。本文档说明换芯片、抽模块时需要改动的范围。

## 三条架构铁律

1. **芯片头 `ti_msp_dl_config.h` 只进 `.c`，不进 `.h`。**
   例外（宏定义直接依赖芯片符号，无法避免）：`hw_delay.h`（`delay_cycles`）、`myiic.h` / `hw_w25qxx.h`（`DL_GPIO_*` 宏）、`mid_wireless_uart.h`（SysConfig 端口宏）、`OLED_driver.h`（平台头宏 `OLED_DRIVER_PLATFORM_HEADER`，可改指向其它芯片配置头）。

2. **下层不依赖上层。** `oledUI/` 核心（`OLED.c` / `OLED_UI.c` / `OLED_Fonts.c` / `OLED_driver.c`）不 include 任何 `hw_*` / `app_*` 头。

3. **跨模块通信走 API 函数（访问器），不用裸 extern 变量。**
   保留的 extern 仅限：只读 ROM 数据表（字模、图标、音符表 `const` 数组）、经接线点读取的 `key_menu`、UI 框架内部变量。

## 换芯片步骤

换到新的 MSPM0 型号（或其它芯片）时，只需改动以下三处：

1. **SysConfig 重新配置外设**，生成新的 `ti_msp_dl_config.c/.h`（I2C/SPI/UART/Timer/GPIO/PWM 等）。

2. **重写 `hardware/hw_*.c` 中的 DriverLib 调用**，使其匹配新芯片外设。`hw_*.h` 接口保持不变，上层无感知。

3. **重写 `oledUI/OLED_UI_Driver.c` 接线点**，把以下适配函数映射到新芯片实现：
   - 按键状态：`Key_GetEnterStatus/Back/Up/Down()`
   - 编码器：`Encoder_Init/Enable/Disable/Get()`
   - 延时：`Delay_us/ms/s()`
   - 定时器/按键初始化：`Timer_Init/Key_Init()`

   非 MSPM0 平台还需改 `OLED_driver.h` 的 `OLED_DRIVER_PLATFORM_HEADER` 宏指向对应配置头，并按需实现 `OLED_driver.c` 的总线后端（硬件 I2C / 软件 I2C）。

## 单独抽取 oledUI 框架

把 `oledUI/` 目录整体拷到新工程后，使其运行起来需要：

1. 实现 `OLED_UI_Driver.h` 中声明的全部适配函数（见上述接线点列表）。

2. 实现 `OLED_driver.h/.c` 的屏幕底层通信（init/update/亮度/颜色模式）。

3. 若使用 app 任务调度，调用 `OLED_UI_SetAppHook(is_active_fn, tick_fn)` 注册任务钩子；不使用则不注册（UI 按"无 app 任务"运行）。

4. `OLED_UI_MenuData.c` 是菜单粘合层（绑定具体业务模块），属于应用级，按新工程的菜单需求重写，不算 UI 核心的一部分。

## 各模块配置访问器一览

重构后，跨模块共享的可变参数统一通过访问器读写：

| 模块 | 访问器 | 返回 | 说明 |
|------|--------|------|------|
| hw_ws2812 | `ws2812_config()` | `WS2812_Config_t*` | RGB 基础参数：enable/r/g/b/led_num/brightness |
| hw_ws2812_effects | `ws2812_effect_param()` | `WS2812_Effect_Param*` | 灯效参数：mode/speed/brightness |
| hw_ws2812_effects | `ws2812_light_mode_ref()` | `int16_t*` | 灯效模式 0=关 1=流水 2=跑马 3=呼吸 |
| hw_lsm6ds3 | `lsm6ds3_angle()` | `Angle*` | 当前姿态角 |
| OLED_UI | `oled_ui_color_mode()` | `bool*` | 颜色模式 true=深色 |
| OLED_UI | `oled_ui_show_fps()` | `bool*` | 是否显示帧率 |
| OLED_UI | `oled_ui_brightness()` | `int16_t*` | 屏幕亮度 |

### 菜单绑定注意事项

UI 菜单框架通过指针绑定变量（`.Prob_Data_Int_16` / `.List_BoolRadioBox`）。由于访问器是函数调用，无法写进全局结构体的静态初始化器（C 语言静态初始化器只能用常量表达式），因此这些绑定在运行时由 `MenuData_Init()` 补写指针。

**`MenuData_Init()` 必须在 `OLED_UI_Init()` 之前调用一次**（见 `empty.c` main 函数）。新增 RGB/配置类菜单项时，记得在 `MenuData_Init()` 里补上对应的指针绑定。

## 已知遗留项

- `Beeper0`（`mid_music`）仍为可变全局 extern，不在本次重构范围内。如需进一步解耦可比照上表改为访问器。
