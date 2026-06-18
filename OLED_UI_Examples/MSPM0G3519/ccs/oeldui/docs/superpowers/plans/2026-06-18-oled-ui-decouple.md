# OLED_UI 工程解耦重构 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 让 `oledUI/` 可零芯片依赖整目录拷出，各模块通过 API/访问器通信而非裸 extern，芯片头收口到 `.c`，换芯片时改动集中到少数文件。

**Architecture:** 沿用 app/middle/hardware/oledUI 四层。三条铁律——芯片头 `ti_msp_dl_config.h` 只进 `.c`、下层不 include 上层、跨模块走 API。手段：接线点模式（适配函数集中在单个 `.c`）+ 配置结构体访问器（方案 A，菜单指针绑定不变）。

**Tech Stack:** TI MSPM0G3519，CCS + tiarmclang，SysConfig，DriverLib。裸机无单元测试框架。

**验证模型（固件适配，无单元测试）：** 每个任务的验证 = ①结构不变量 grep 检查（执行者可直接跑）+ ②CCS 编译通过（用户跑，本地无工具链）+ ③硬件功能回归（用户跑）。提交前至少通过 ① 与 ②。

**对应 spec：** `docs/superpowers/specs/2026-06-18-oled-ui-decouple-design.md`

---

## 文件结构总览

本计划改动的文件及职责：

- `oledUI/OLED_UI_Driver.h` — UI 核心需要的抽象接口声明，**零芯片/零上层依赖**。
- `oledUI/OLED_UI_Driver.c` — 唯一接线点，include 上层头并实现适配函数。
- `oledUI/OLED_UI.c` / `.h` — UI 配置（ColorMode/ShowFps/Brightness）收进访问器。
- `hardware/hw_ws2812.c` / `.h` — WS2812 基础参数收进 `WS2812_Config_t` + `ws2812_config()`。
- `hardware/hw_ws2812_effects.c` / `.h` — `effect_param` / `ws2812_light_mode` 改访问器。
- `hardware/hw_lsm6ds3.c` / `.h` — `angle_new` 改访问器。
- `middle/mid_music.c` / `.h` — `MySound` 降级为 static。
- `oledUI/OLED_UI_MenuData.c` — 菜单绑定改 `&xxx_config()->field`，settings 打包/解包同步改。
- 各 `*.h`（hw_/mid_/app_key_task）— 移除 `#include "ti_msp_dl_config.h"`。

---

# 阶段 1：OLED_UI 核心解耦

### Task 1: OLED_UI_Driver 接口与实现分离

**Files:**
- Modify: `oledUI/OLED_UI_Driver.h`
- Modify: `oledUI/OLED_UI_Driver.c`

**背景：** 当前 `OLED_UI_Driver.h` include 了 `ti_msp_dl_config.h` / `hw_key.h` / `app_key_task.h`，并用宏 `#define Key_GetEnterStatus() key_menu.enter` 直读 app 层全局结构体。这使 `oledUI/` 反向依赖 `app/`，整目录拿不出来。

- [ ] **Step 1: 改写 OLED_UI_Driver.h，删除耦合 include，宏改函数声明**

将 `oledUI/OLED_UI_Driver.h` 顶部的：

```c
#include "ti_msp_dl_config.h"           // Device header
#include "hw_key.h"
#include "app_key_task.h"

//按下0 松开1
#define Key_GetEnterStatus()    key_menu.enter
#define Key_GetBackStatus()     key_menu.back
#define Key_GetUpStatus()       key_menu.up
#define Key_GetDownStatus()     key_menu.down
```

替换为（不再 include 任何芯片头/上层头，仅标准类型）：

```c
#include <stdint.h>

// 按键状态读取（按下0 松开1）。实现见 OLED_UI_Driver.c，移植时只改该 .c。
uint8_t Key_GetEnterStatus(void);
uint8_t Key_GetBackStatus(void);
uint8_t Key_GetUpStatus(void);
uint8_t Key_GetDownStatus(void);
```

其余函数声明（`Timer_Init` / `Key_Init` / `Encoder_*` / `Delay_*`）保持不变。

- [ ] **Step 2: 在 OLED_UI_Driver.c 中实现这 4 个按键函数**

在 `oledUI/OLED_UI_Driver.c` 顶部 include 区补上上层头（接线点允许 include 上层）：

```c
#include "OLED_UI_Driver.h"
#include "hw_delay.h"
#include "hw_encoder.h"
#include "hw_key.h"
#include "app_key_task.h"
```

并在文件中新增实现（放在 Encoder/Delay 函数附近）：

```c
/* 按键状态适配：UI 核心只调本组函数，移植时改这里即可 */
uint8_t Key_GetEnterStatus(void) { return key_menu.enter; }
uint8_t Key_GetBackStatus(void)  { return key_menu.back;  }
uint8_t Key_GetUpStatus(void)    { return key_menu.up;    }
uint8_t Key_GetDownStatus(void)  { return key_menu.down;  }
```

- [ ] **Step 3: 结构不变量检查（执行者跑）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "应为空: oledUI 头文件不得 include 芯片头/上层头"
grep -nE 'ti_msp_dl_config\.h|hw_key\.h|app_key_task\.h' oledUI/OLED_UI_Driver.h
echo "应有 4 行函数声明:"
grep -nE 'uint8_t Key_Get(Enter|Back|Up|Down)Status\(void\);' oledUI/OLED_UI_Driver.h
```
Expected: 第一个 grep 无输出；第二个 grep 输出 4 行声明。

- [ ] **Step 4: CCS 编译验证（用户跑）**

用户在 CCS 中 Build。Expected: 编译通过，无 `key_menu` 未定义、无 `Key_GetEnterStatus` 重定义错误。

- [ ] **Step 5: 提交**

```bash
git add oledUI/OLED_UI_Driver.h oledUI/OLED_UI_Driver.c
git commit -m "refactor(oledui): #260618 OLED_UI_Driver 接口与实现分离,UI核心去除芯片/上层依赖"
```

---

### Task 2: 验证 oledUI 目录对外依赖收敛

**Files:**
- 只读检查，无修改（如发现残留耦合则在本任务修复）

- [ ] **Step 1: 扫描 oledUI 全目录的越界 include**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "=== oledUI/*.h 中是否还有芯片头(应为空) ==="
grep -rnE 'ti_msp_dl_config\.h' oledUI/*.h
echo "=== oledUI 中 include 上层(hw_/app_)的非接线点文件 ==="
grep -rnE '#include "(hw_|app_)' oledUI/*.c oledUI/*.h | grep -v 'OLED_UI_Driver.c'
```
Expected: 第一个 grep 无输出；第二个 grep 理想为空。若 `OLED_UI_MenuData.c` 出现 `hw_ws2812.h`（阶段 2 处理）可暂时保留，记录但不在本任务改。

- [ ] **Step 2: 记录结论**

在提交信息或 commit body 中记录：阶段 1 完成后 `oledUI/` 核心（OLED.c/OLED_UI.c/Driver）对外仅依赖 `OLED_UI_Driver.h` 函数声明；`OLED_UI_MenuData.c` 对 hardware 的依赖留待阶段 2 通过访问器收敛。本任务无代码改动则无需 commit。

---

# 阶段 2：消除 extern 全局，改配置结构体 + 访问器

### Task 3: hw_ws2812 基础参数收进 WS2812_Config_t

**Files:**
- Modify: `hardware/hw_ws2812.h`
- Modify: `hardware/hw_ws2812.c`

**背景：** `hw_ws2812.c` 当前裸定义 `ws2812_enable/r/g/b/led_num/brightness` 6 个全局，`.h` 用 `extern` 暴露。

- [ ] **Step 1: 在 hw_ws2812.h 用结构体+访问器替换 6 个裸 extern**

将 `hardware/hw_ws2812.h` 中：

```c
/* RGB控制参数（供菜单UI绑定） */
extern bool     ws2812_enable;   // RGB灯总开关
extern int16_t  ws2812_r;        // 红色分量 0-255
extern int16_t  ws2812_g;        // 绿色分量 0-255
extern int16_t  ws2812_b;        // 蓝色分量 0-255
extern int16_t  ws2812_led_num;  // 亮灯个数 1-4
extern int16_t  ws2812_brightness; // 灯光亮度 0-100
extern WS2812_Effect_Param effect_param;  // 效果参数
```

替换为：

```c
/* RGB 控制参数（供菜单 UI 通过 &ws2812_config()->field 绑定）*/
typedef struct {
    bool     enable;       // RGB灯总开关
    int16_t  r;            // 红色分量 0-255
    int16_t  g;            // 绿色分量 0-255
    int16_t  b;            // 蓝色分量 0-255
    int16_t  led_num;      // 亮灯个数 1-4
    int16_t  brightness;   // 灯光亮度 0-100
} WS2812_Config_t;

WS2812_Config_t* ws2812_config(void);   // 获取配置句柄
```

注：`effect_param` 的 extern 移到 Task 4 由 effects 模块提供访问器，这里一并删除该行。

- [ ] **Step 2: 在 hw_ws2812.c 定义结构体实例与访问器，并替换内部引用**

将 `hardware/hw_ws2812.c` 顶部：

```c
bool     ws2812_enable  = true;
int16_t  ws2812_r       = 0;
int16_t  ws2812_g       = 0;
int16_t  ws2812_b       = 0;
int16_t  ws2812_led_num = 4;
int16_t  ws2812_brightness = 100;
```

替换为：

```c
static WS2812_Config_t s_ws2812_cfg = {
    .enable = true, .r = 0, .g = 0, .b = 0, .led_num = 4, .brightness = 100,
};

WS2812_Config_t* ws2812_config(void) { return &s_ws2812_cfg; }
```

然后把 `hw_ws2812.c` 内所有 `ws2812_enable` → `s_ws2812_cfg.enable`、`ws2812_r` → `s_ws2812_cfg.r`、`ws2812_g/b/led_num/brightness` 同理替换（约在第 189-210 行的 `ws2812_update()` 等处）。

- [ ] **Step 3: 结构不变量检查（执行者跑）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "应为空: hw_ws2812.h 不再有这些裸 extern"
grep -nE 'extern (bool|int16_t).*ws2812_(enable|r|g|b|led_num|brightness)' hardware/hw_ws2812.h
echo "应为空: hw_ws2812.c 不再有裸全局定义(只剩 s_ws2812_cfg)"
grep -nE '^(bool|int16_t)\s+ws2812_(enable|r|g|b|led_num|brightness)' hardware/hw_ws2812.c
echo "应有: 访问器"
grep -n 'WS2812_Config_t\* ws2812_config' hardware/hw_ws2812.c
```
Expected: 前两个无输出，第三个有 1 行。

- [ ] **Step 4: 编译会暂时失败（MenuData/effects 仍引用旧名）—— 留到 Task 6 统一修复后再 CCS 验证**

本任务先不要求 CCS 通过；阶段 2 内部跨文件引用在 Task 6 收口后一起编译验证。执行者继续下一任务。

- [ ] **Step 5: 暂不提交**，与 Task 4/5/6 合并为一次阶段 2 提交（见 Task 6 Step 4）。

---

### Task 4: hw_ws2812_effects 的 effect_param / light_mode 改访问器

**Files:**
- Modify: `hardware/hw_ws2812_effects.h`
- Modify: `hardware/hw_ws2812_effects.c`

- [ ] **Step 1: 改 hw_ws2812_effects.h**

将：

```c
extern WS2812_Effect_Param effect_param;
extern int16_t ws2812_light_mode;  /* 0=关, 1=流水灯, 2=跑马灯, 3=呼吸灯 */
```

替换为：

```c
WS2812_Effect_Param* ws2812_effect_param(void);   // 效果参数句柄(供菜单绑定 &...->speed)
int16_t*             ws2812_light_mode_ref(void); // 灯效模式句柄 0=关 1=流水 2=跑马 3=呼吸
```

- [ ] **Step 2: 改 hw_ws2812_effects.c**

将（约第 5、12 行）：

```c
WS2812_Effect_Param effect_param = {
    /* ...原初值保持不变... */
};
int16_t ws2812_light_mode = 0;
```

改为在原定义前加 `static`，并补访问器：

```c
static WS2812_Effect_Param effect_param = {
    /* ...原初值保持不变... */
};
static int16_t ws2812_light_mode = 0;

WS2812_Effect_Param* ws2812_effect_param(void) { return &effect_param; }
int16_t*             ws2812_light_mode_ref(void) { return &ws2812_light_mode; }
```

文件内其余对 `effect_param` / `ws2812_light_mode` 的直接引用保持不变（同文件 static 可见）。

- [ ] **Step 3: 结构不变量检查**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "应为空:"
grep -nE 'extern (WS2812_Effect_Param effect_param|int16_t ws2812_light_mode)' hardware/hw_ws2812_effects.h
echo "应有 2 个访问器:"
grep -nE 'ws2812_(effect_param|light_mode_ref)\(void\)' hardware/hw_ws2812_effects.c
```
Expected: 第一个无输出，第二个 2 行。

- [ ] **Step 4: 暂不提交**（并入 Task 6）。

---

### Task 5: hw_lsm6ds3 的 angle_new 改访问器 & mid_music 的 MySound 降级 static

**Files:**
- Modify: `hardware/hw_lsm6ds3.h`
- Modify: `hardware/hw_lsm6ds3.c`
- Modify: `middle/mid_music.h`
- Modify: `middle/mid_music.c`

- [ ] **Step 1: hw_lsm6ds3.h —— angle_new 改访问器**

将 `hardware/hw_lsm6ds3.h:109`：

```c
extern Angle angle_new;
```

替换为：

```c
Angle* lsm6ds3_angle(void);   // 当前姿态角句柄
```

- [ ] **Step 2: hw_lsm6ds3.c —— 定义改 static + 访问器**

将 `hardware/hw_lsm6ds3.c:12`：

```c
Angle angle_new;
```

替换为：

```c
static Angle angle_new;
Angle* lsm6ds3_angle(void) { return &angle_new; }
```

文件内对 `angle_new` 的直接引用保持不变。

- [ ] **Step 3: 处理 angle_new 的外部使用方**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "=== angle_new 在 hw_lsm6ds3.c 之外的引用 ==="
grep -rnE '\bangle_new\b' --include="*.c" --include="*.h" app middle oledUI hardware | grep -v 'hw_lsm6ds3.c'
```
对每个外部引用点（如 `app_gyroscope.c`），把 `angle_new.xxx` 改为 `lsm6ds3_angle()->xxx`，并确保该文件 include 了 `hw_lsm6ds3.h`。逐处替换（具体行由上面 grep 输出确定）。

- [ ] **Step 4: mid_music 的 MySound 降级 static**

`MySound` 仅在 `mid_music.c/.h` 内使用，无外部引用。
- 删除 `middle/mid_music.h:98` 的 `extern TONE *MySound;`（连同其所在行）。
- 将 `middle/mid_music.c:77` 的 `TONE *MySound;` 改为 `static TONE *MySound;`。

- [ ] **Step 5: 结构不变量检查**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "应为空: angle_new / MySound 不再有 extern"
grep -rnE 'extern (Angle angle_new|TONE \*MySound)' hardware/hw_lsm6ds3.h middle/mid_music.h
echo "应为空: angle_new 在 hw_lsm6ds3.c 外不再裸用(应改 lsm6ds3_angle())"
grep -rnE '\bangle_new\b' --include="*.c" app middle oledUI | grep -v 'hw_lsm6ds3.c'
```
Expected: 两个 grep 均无输出。

- [ ] **Step 6: 暂不提交**（并入 Task 6）。

---

### Task 6: OLED_UI 自身配置访问器 + MenuData 绑定改写 + 阶段 2 编译验证

**Files:**
- Modify: `oledUI/OLED_UI.c`
- Modify: `oledUI/OLED_UI.h`
- Modify: `oledUI/OLED_UI_MenuData.c`
- Modify: `empty.c`（main 中的 extern 同步改）

**背景：** `OLED_UI.c` 定义 `ColorMode/OLED_UI_ShowFps/OLED_UI_Brightness` 三个全局，被 `OLED_UI_MenuData.c` 和 `empty.c` 以 extern 引用。MenuData 还以指针绑定全部参数。本任务统一收口并编译验证整个阶段 2。

- [ ] **Step 1: OLED_UI.h 暴露三个配置访问器**

在 `oledUI/OLED_UI.h` 适当位置（显存 extern 附近）新增声明：

```c
// UI 显示配置访问器(供菜单绑定)
bool*    OLED_UI_ColorMode_ref(void);   // true=深色 DARKMODE
bool*    OLED_UI_ShowFps_ref(void);
int16_t* OLED_UI_Brightness_ref(void);
```

保留 `ColorMode` 等的现有定义方式不变（见下一步决定是否 static 化）。

> 设计决定：`ColorMode` 在 UI 框架内部被多处直接读（渲染反色逻辑），若改 static 需在 OLED_UI.c 内全部可见——它们本就定义在 OLED_UI.c，改 static 不影响同文件引用。故采用：定义加 `static`，对外仅暴露访问器。

- [ ] **Step 2: OLED_UI.c 定义改 static + 访问器**

将 `oledUI/OLED_UI.c:20-22`：

```c
bool ColorMode = DARKMODE;
bool OLED_UI_ShowFps = true;
int16_t OLED_UI_Brightness = 100;
```

替换为：

```c
static bool ColorMode = DARKMODE;
static bool OLED_UI_ShowFps = true;
static int16_t OLED_UI_Brightness = 100;

bool*    OLED_UI_ColorMode_ref(void)  { return &ColorMode; }
bool*    OLED_UI_ShowFps_ref(void)    { return &OLED_UI_ShowFps; }
int16_t* OLED_UI_Brightness_ref(void) { return &OLED_UI_Brightness; }
```

OLED_UI.c 内对这三个变量的其余引用保持不变（同文件 static 可见）。

- [ ] **Step 3: 检查 ColorMode/ShowFps/Brightness 在 OLED_UI.c 之外的引用并改访问器**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
grep -rnE '\b(ColorMode|OLED_UI_ShowFps|OLED_UI_Brightness)\b' --include="*.c" app middle oledUI | grep -v 'oledUI/OLED_UI.c'
```
对每处外部引用（OLED.c 渲染、MenuData、empty.c 等）：
- 读值：`ColorMode` → `*OLED_UI_ColorMode_ref()`
- 取址绑定菜单：`&ColorMode` → `OLED_UI_ColorMode_ref()`

> 注意 `OLED.c` 若直接用 `ColorMode` 做反色渲染，需 include `OLED_UI.h` 并改用 `*OLED_UI_ColorMode_ref()`。逐处按 grep 输出替换。

- [ ] **Step 4: 改写 OLED_UI_MenuData.c 的全部绑定与 settings 打包**

删除文件顶部的 extern 声明（第 21-27 行附近）：

```c
extern bool ColorMode;
extern bool OLED_UI_ShowFps;
extern int16_t OLED_UI_Brightness;
extern WS2812_Effect_Param effect_param;
extern int16_t ws2812_light_mode;
```

（`hw_ws2812.h` / `hw_ws2812_effects.h` / `OLED_UI.h` 已提供访问器，保留这些 `#include`。）

菜单项指针绑定逐项改写：

| 原绑定 | 改为 |
|--------|------|
| `&OLED_UI_Brightness` | `OLED_UI_Brightness_ref()` |
| `&ColorMode` | `OLED_UI_ColorMode_ref()` |
| `&OLED_UI_ShowFps` | `OLED_UI_ShowFps_ref()` |
| `&ws2812_enable` | `&ws2812_config()->enable` |
| `&ws2812_r` | `&ws2812_config()->r` |
| `&ws2812_g` | `&ws2812_config()->g` |
| `&ws2812_b` | `&ws2812_config()->b` |
| `&ws2812_led_num` | `&ws2812_config()->led_num` |
| `&ws2812_brightness` | `&ws2812_config()->brightness` |
| `(int16_t*)&effect_param.speed` | `&ws2812_effect_param()->speed` |
| `&ws2812_light_mode` | `ws2812_light_mode_ref()` |

`ToggleColorMode()` 函数体内 `ColorMode = !ColorMode;` → `bool* cm = OLED_UI_ColorMode_ref(); *cm = !*cm;`

settings 打包段（约第 142-154 行）逐行改：

```c
temp[0]  = (uint8_t)(*OLED_UI_Brightness_ref());
temp[3]  = (uint8_t)(*OLED_UI_ColorMode_ref());
temp[4]  = (uint8_t)(*OLED_UI_ShowFps_ref());
temp[5]  = (uint8_t)(ws2812_config()->enable);
temp[6]  = (uint8_t)(ws2812_config()->r);
temp[7]  = (uint8_t)(ws2812_config()->g);
temp[8]  = (uint8_t)(ws2812_config()->b);
temp[9]  = (uint8_t)(ws2812_config()->led_num);
temp[10] = (uint8_t)(*ws2812_light_mode_ref());
temp[11] = (uint8_t)(ws2812_effect_param()->speed);
temp[12] = (uint8_t)(ws2812_config()->brightness);
```

同步检查 settings **解包/load** 段（settings_load 处），把对应 `ws2812_r = ...` / `ColorMode = ...` 等写回也改成 `ws2812_config()->r = ...` / `*OLED_UI_ColorMode_ref() = ...`。先 grep 定位：

```bash
grep -nE 'ws2812_(enable|r|g|b|led_num|brightness)|ColorMode|OLED_UI_(ShowFps|Brightness)|ws2812_light_mode|effect_param' oledUI/OLED_UI_MenuData.c
```

- [ ] **Step 5: empty.c 同步**

```bash
grep -nE 'extern .*(ColorMode|OLED_UI_ShowFps|OLED_UI_Brightness)' empty.c
```
删除 `empty.c` 中这些 extern 声明，把 main 内对它们的使用改为 `*OLED_UI_xxx_ref()`（include `OLED_UI.h` 已有）。

- [ ] **Step 6: 阶段 2 全量结构检查（执行者跑）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "=== 阶段2目标:全工程不应再有这些裸 extern 变量(函数 extern 除外) ==="
grep -rnE 'extern .*(ws2812_enable|ws2812_r|ws2812_g|ws2812_b|ws2812_led_num|ws2812_brightness|effect_param|ws2812_light_mode|angle_new|ColorMode|OLED_UI_ShowFps|OLED_UI_Brightness|MySound)\b' --include="*.c" --include="*.h" app middle hardware oledUI empty.c | grep -vE '\(void\)|\('
```
Expected: 无输出（全部已封装为访问器）。

- [ ] **Step 7: CCS 编译验证（用户跑）—— 覆盖 Task 3~6**

用户 Build。Expected: 编译通过，无未定义符号、无类型不匹配。若报错按符号名定位到上述某张映射表漏改处补齐。

- [ ] **Step 8: 硬件功能回归（用户测）**

逐项确认：RGB 开关/RGB 红绿蓝调节/灯数/亮度/灯效模式切换、颜色模式切换、FPS 显示开关、屏幕亮度滑条、姿态角显示、音乐播放、settings 掉电保存与重启恢复。

- [ ] **Step 9: 阶段 2 提交**

```bash
git add hardware/hw_ws2812.c hardware/hw_ws2812.h hardware/hw_ws2812_effects.c hardware/hw_ws2812_effects.h hardware/hw_lsm6ds3.c hardware/hw_lsm6ds3.h middle/mid_music.c middle/mid_music.h oledUI/OLED_UI.c oledUI/OLED_UI.h oledUI/OLED_UI_MenuData.c empty.c
git commit -m "refactor(modules): #260618 消除跨文件extern,各模块改配置结构体+访问器(方案A)"
```

---

# 阶段 3：芯片头清出所有 .h

### Task 7: 将 ti_msp_dl_config.h 从所有 .h 移到对应 .c

**Files（依 Task 探查清单）:** `app_key_task.h`、`mid_debug_led.h`、`mid_debug_uart.h`、`mid_font_burner.h`、`mid_music.h`、`mid_timer.h`、`mid_wireless_uart.h`、`hw_buzzer.h`、`hw_delay.h`、`hw_encoder.h`、`hw_key.h`、`hw_w25qxx.h`、`hw_ws2812.h`、`myiic.h`、`oledUI/oled_ext_font.h` 及它们对应的 `.c`。

- [ ] **Step 1: 列出所有在 .h 中 include 芯片头的文件（执行者跑，确认清单）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
grep -rln 'ti_msp_dl_config.h' --include="*.h" app middle hardware oledUI
```

- [ ] **Step 2: 对每个 .h 逐一处理**

对清单中每个 `xxx.h`：
1. 删除 `xxx.h` 中的 `#include "ti_msp_dl_config.h"`。
2. 检查 `xxx.h` 是否用到芯片类型（如 `GPIO_Regs*`、`DL_*`、外设句柄）。
   - 若头里只用了 `uint8_t/int16_t/bool`，补 `#include <stdint.h>` / `#include <stdbool.h>` 即可。
   - 若头里的函数原型确实需要芯片类型参数，优先改用标准类型（如句柄改 `void*`）或将该原型相关定义移入 `.c`；若无法避免，则该头保留芯片依赖并在 spec 风险中记录例外（应为极少数）。
3. 在对应 `xxx.c` 顶部确保有 `#include "ti_msp_dl_config.h"`（通常已有；没有则补上，放在 `#include "xxx.h"` 之后）。

逐文件提交粒度：可每改 3-4 个文件做一次中间 CCS 编译，降低定位成本。

- [ ] **Step 3: 结构不变量检查（执行者跑）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "=== 阶段3目标:任何 .h 都不得再 include 芯片头(应为空) ==="
grep -rn 'ti_msp_dl_config.h' --include="*.h" app middle hardware oledUI
```
Expected: 无输出。

- [ ] **Step 4: CCS 编译验证（用户跑）**

用户 Build。Expected: 编译通过。常见报错为某 `.c` 漏补芯片头 include，按报错文件补上。

- [ ] **Step 5: 提交**

```bash
git add -A
git commit -m "refactor(headers): #260618 芯片头ti_msp_dl_config.h清出所有.h,仅保留于.c"
```

---

# 阶段 4：收尾验证与移植说明

### Task 8: 全量结构验证 + 移植文档

**Files:**
- Create: `docs/PORTING.md`

- [ ] **Step 1: 全量结构不变量总检（执行者跑）**

```bash
cd "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3519/ccs/oeldui"
echo "=== 1. 任何 .h 不得含芯片头(应为空) ==="
grep -rn 'ti_msp_dl_config.h' --include="*.h" app middle hardware oledUI
echo "=== 2. 不得有跨文件裸 extern 变量(函数除外,应为空) ==="
grep -rnE '^\s*extern\s+(bool|int8_t|uint8_t|int16_t|uint16_t|int32_t|uint32_t|float|double|Angle|TONE|WS2812)' --include="*.h" --include="*.c" app middle hardware oledUI empty.c | grep -vE '\(' 
echo "=== 3. oledUI 核心不依赖 hw_/app_(仅 Driver.c 例外) ==="
grep -rnE '#include "(hw_|app_)' oledUI/*.c oledUI/*.h | grep -v 'OLED_UI_Driver.c'
```
Expected: 三项均无输出（或仅剩显存 `OLED_DisplayBuf` 这类合理项，记录说明）。

- [ ] **Step 2: 完整 CCS 编译 + 烧录全功能回归（用户）**

菜单导航、RGB 全部参数、颜色/FPS/亮度、姿态显示、四款游戏（bird/dino/brick/plane/snake）、按键、编码器、无线串口、settings 保存恢复。

- [ ] **Step 3: 写移植说明 docs/PORTING.md**

内容要点（基于本次重构结论）：
- 三条铁律。
- 换芯片步骤：① SysConfig 重新配置外设并生成 `ti_msp_dl_config.c/.h`；② 重写 `hardware/hw_*.c` 中 DriverLib 调用；③ 重写 `oledUI/OLED_UI_Driver.c` 接线点（按键/编码器/延时映射）。
- 单独抽取 `oledUI/` 的方法：整目录拷出 + 实现 `OLED_UI_Driver.h` 中声明的函数 + 提供 `OLED_UI_xxx_ref()` 配置访问器。
- 各模块访问器一览表（ws2812_config / ws2812_effect_param / ws2812_light_mode_ref / lsm6ds3_angle / OLED_UI_*_ref）。

- [ ] **Step 4: 提交**

```bash
git add docs/PORTING.md
git commit -m "docs(porting): #260618 新增移植说明,记录解耦后换芯片与抽模块步骤"
```

---

## 计划自检结论

- **Spec 覆盖**：阶段1(Task1-2)↔spec阶段1；阶段2(Task3-6)↔spec阶段2；阶段3(Task7)↔spec阶段3；阶段4(Task8)↔spec阶段4。三条铁律分别由 Task1/7、Task3-6、全程落实。
- **占位符**：无 TBD/TODO；每个代码步骤含具体代码或精确 grep。
- **类型一致性**：访问器命名全程统一——`ws2812_config()→WS2812_Config_t*`、`ws2812_effect_param()→WS2812_Effect_Param*`、`ws2812_light_mode_ref()→int16_t*`、`lsm6ds3_angle()→Angle*`、`OLED_UI_ColorMode_ref()/ShowFps_ref()/Brightness_ref()`。Task6 菜单绑定与 Task3-5 定义一致。
- **验证模型**：每任务先跑结构 grep（执行者），编译与硬件回归由用户在 CCS 完成；阶段2因跨文件依赖在 Task6 统一编译验证。
