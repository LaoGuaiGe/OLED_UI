## 例子总结

**开发环境**

该工程基于 TI（德州仪器）的 MSPM0G3507 制作。使用 CCS20.1.1 编译，XDS110 烧录。

**实现功能**

通过两个按键控制菜单的动作：
* 左键为back与up键，back为长按效果，up为单击效果。
* 右键为enter与down键，enter为长按效果，down为单击效果。

> 按键功能基于开源项目 [MultiButton](https://github.com/0x1abin/MultiButton) 移植，实现单击与长按效果。

## 外设与引脚配置

| 外设 | 引脚 | 功能 |
| --- | --- | --- |
| 蜂鸣器 | PB27 | 声音提醒 |
| 左键 | PA18 | BACK 和 UP 键 |
| 右键 | PB21 | ENTER 和 DOWN 键 |
| I2C屏幕的SDA | PA0 | 单色I2C屏幕通信数据引脚 |
| I2C屏幕的SCL | PA1 | 单色I2C屏幕通信时钟引脚 |
| UART0-TX | PA10 | 串口调试-串口发送引脚 |
| UART0-RX | PA11 | 串口调试-串口接收引脚 |
| DEBUGSS | PA20 | SW烧录 SWCLK 时钟引脚 |
| DEBUGSS | PA19 | SW烧录 SWDIO 数据引脚 |
| 指示灯 | PB22 | 低电平亮指示灯 |

## 移植建议

## 其他


---

## Example Summary

**Development Environment​**

The project is developed based on TI's MSPM0G3507 microcontroller. It was compiled using CCS (Code Composer Studio) version 20.1.1 and programmed via an XDS110 debug probe.

**​​Feature Implementation​**

Controlling menu actions with two buttons:

* The left button serves as ​​Back​​ (long press) and ​​Up​​ (single click).

* The right button serves as ​​Enter​​ (long press) and ​​Down​​ (single click).

> The button functionality is implemented based on the open-source project [MultiButton](https://github.com/0x1abin/MultiButton), supporting single click and long press effects.

## Peripherals & Pin Assignments

| Peripheral       | Pin   | Function                                  |
| :--------------- | :---- | :----------------------------------------- |
| Buzzer           | PB27  | Sound reminder                             |
| Left Button      | PA18  | BACK and UP button                         |
| Right Button     | PB21  | ENTER and DOWN button                      |
| I2C Screen SDA   | PA0   | Data pin for monochrome I2C screen communication |
| I2C Screen SCL   | PA1   | Clock pin for monochrome I2C screen communication |
| UART0-TX         | PA10  | Serial port debugging - Serial transmit pin |
| UART0-RX         | PA11  | Serial port debugging - Serial receive pin |
| DEBUG SS         | PA20  | SW programming - SWCLK clock pin           |
| DEBUG SS         | PA19  | SW programming - SWDIO data pin            |
| Indicator Light  | PB22  | Indicator light (on at low level)         |

## Device Migration Recommendations


## FAQ