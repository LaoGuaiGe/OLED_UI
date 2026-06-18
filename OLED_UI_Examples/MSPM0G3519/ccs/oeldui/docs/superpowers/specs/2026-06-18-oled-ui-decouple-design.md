# OLED_UI 工程解耦重构设计

日期：2026-06-18
状态：已确认，待审阅

## 背景与目标

当前 OLED_UI 工程（TI MSPM0G3519，CCS）虽已有 `app/middle/hardware/oledUI` 四层划分，但存在严重耦合，导致移植适配性低、模块无法单独抽出：

- **芯片头渗透**：`ti_msp_dl_config.h`（SysConfig 生成）被直接 `#include` 进 14+ 个 `.h` 文件，换芯片时全部要改。
- **跨文件 extern 全局变量**：共享变量通过裸 `extern` 散落各处，违反封装。
- **UI 核心反向依赖上层**：`oledUI/OLED_UI_Driver.h` 同时 include 芯片头 + `hw_key.h` + `app_key_task.h`，并用宏 `key_menu.enter` 直读 app 层全局结构体，导致 `oledUI/` 拿不出来单独用。

**目标**：在不引入新目录层级的前提下，通过"芯片隔离 + 适配函数 + 配置访问器"三种手段，使 `oledUI/` 可零芯片依赖整目录拷出，各模块通过 API 通信而非裸全局变量，换芯片时改动收口到少数文件。

## 分层架构与依赖规则

沿用现有四层，不新增 `port/` 层（MSPM0 的 SysConfig 生成的 `ti_msp_dl_config.c/.h` 加上 `hardware/hw_*.c` 已经充当芯片适配层，再加一层是多余转发）：

```
app/       业务页面、游戏
oledUI/    UI 框架(显示+菜单)  —— 零芯片依赖,纯算法
middle/    中间件(按键库/音乐/姿态融合)
hardware/  芯片外设驱动 (hw_*) —— 唯一碰芯片头的地方,且只在 .c 里碰
```

**三条铁律：**

1. **芯片头 `ti_msp_dl_config.h` 只允许出现在 `hardware/*.c`（及个别 `.c` 接线点），绝不进任何 `.h`。**
2. **下层永不 include 上层**；`oledUI/` 不准 include `hw_*.h` / `app_*.h`。
3. **跨模块通信一律走 API 函数（含配置访问器），禁止 `extern` 全局变量。**

## 解耦手段

### 手段一：适配函数（接线点模式）

推广现有 `OLED_UI_Driver.c` 已有的范式——把芯片相关操作收口成一组函数（`Encoder_Get()` / `Delay_ms()` / `Key_GetEnterStatus()`），上层只调函数声明，具体实现集中在单个"接线点" `.c` 文件。换芯片时只改接线点 `.c`。

### 手段二：配置结构体 + 访问器（方案 A，消除 extern）

菜单框架是**按指针绑定变量**的（`.Prob_Data_Int_16 = &ws2812_r`、`.List_BoolRadioBox = &ColorMode`），不直接兼容纯 getter/setter。因此采用配置结构体方案，不改 UI 框架核心：

每个模块把自己的参数收进一个结构体，定义在 `.c` 里，只暴露一个访问器函数：

```c
// hw_ws2812.h —— 只暴露访问器,不再有裸 extern
typedef struct { bool enable; int16_t r, g, b, led_num, brightness; } WS2812_Config_t;
WS2812_Config_t* ws2812_config(void);   // 取配置句柄
void ws2812_apply(void);                // 参数改后刷新硬件
```

菜单绑定改为：`.Prob_Data_Int_16 = &ws2812_config()->r`。

- 跨文件符号从 N 个裸全局压成 1 个访问器；变量归属权回到模块内部；菜单指针机制原封不动，风险低。

## 分阶段实施计划

每阶段做完都能单独编译验证，不会一次性推翻全工程。

### 阶段 1：OLED_UI 核心解耦（价值最高）

- `oledUI/OLED_UI_Driver.h`：删掉 `ti_msp_dl_config.h` / `hw_key.h` / `app_key_task.h` 三个 include；`Key_Get*Status()` 宏改成函数声明（`uint8_t Key_GetEnterStatus(void)` 等）。
- `oledUI/OLED_UI_Driver.c`：成为唯一接线点，include 上层头（`app_key_task.h` 等）并实现这些函数。
- **验证目标**：`oledUI/` 目录对外只依赖 `OLED_UI_Driver.h` 的函数声明，理论上可整目录拷到新工程。

> 注：宏 `key_menu.enter` 改函数有极小调用开销，但对 20ms 刷新一次的菜单按键完全无影响。原注释"用宏为效率"的理由在此不成立。

### 阶段 2：消除 extern 全局，改配置结构体 + 访问器（方案 A）

- `hw_ws2812`：`ws2812_enable / r / g / b / led_num / brightness` → `WS2812_Config_t` + `ws2812_config()`；`effect_param` / `ws2812_light_mode` 一并收入。
- `hw_lsm6ds3`：`angle_new` → 访问器。
- `OLED_UI` 自身的 `ColorMode / OLED_UI_ShowFps / OLED_UI_Brightness`：收进 UI 配置访问器。
- `mid_music` 的 `MySound`：访问器处理。
- `oledUI/OLED_UI_MenuData.c`：菜单绑定改成 `&xxx_config()->field` 形式；settings 存取（`temp[]` 打包）同步改。
- **验证目标**：全工程 `grep "extern .*;"`（非函数声明）结果清零或只剩合理项（如显存 `OLED_DisplayBuf`）。

### 阶段 3：芯片头清出所有 .h（铁律 1）

- 把 `hw_*.h` / `mid_*.h` / `app_key_task.h` 等里的 `#include "ti_msp_dl_config.h"` 移到对应 `.c`；`.h` 中用到的芯片类型改用 `<stdint.h>` / `<stdbool.h>` 标准类型。
- **验证目标**：`grep -r ti_msp_dl_config.h --include=*.h` 结果为空。

### 阶段 4：收尾验证

- CCS 编译通过，功能回归（菜单 / RGB / 姿态 / 游戏 / 按键 / 编码器）。
- 更新移植说明：换芯片时只需动 `hardware/*.c` + `oledUI/OLED_UI_Driver.c` + SysConfig 重新生成。

## 风险与回归

- 每阶段独立可编译，逐阶段提交，便于回退。
- 宏改函数：极小开销，无功能影响。
- 配置结构体访问器：菜单指针绑定语义不变，需逐项核对 `&config()->field` 地址正确性，尤其 settings 存取打包/解包顺序。
- 功能回归重点：RGB 各参数调节、颜色模式/FPS 开关、亮度滑条、姿态角显示、音乐播放、四款游戏、按键与编码器输入。

## 非目标（YAGNI）

- 不新增 `port/` 目录层。
- 不改造 UI 菜单框架核心（不引入 getter/setter 回调式菜单项）。
- 不做与解耦无关的重构（如游戏逻辑优化、字体系统重写）。
