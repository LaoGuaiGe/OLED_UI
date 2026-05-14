# OLED_UI Web IDE

基于 [OLED_UI](https://github.com/bdth-7777777/OLED_UI) 框架的可视化菜单编辑器，支持实时预览、中文显示和 C 代码导出。

## 功能

- 实时 128×64 OLED 屏幕模拟（Canvas 4x 放大）
- 5 种菜单类型预览：列表、磁贴、景深磁贴、HOPE 磁贴、弧形磁贴
- 可视化编辑菜单树结构（添加/删除/排序页面和菜单项）
- 属性面板调整：菜单类型、字体大小、光标样式、动画速度等
- 弹窗/进度条窗口预览
- GB2312 中文字库支持（HZK12/16/20）
- 一键导出 C 代码（可直接粘贴到 OLED_UI 项目）
- 项目 JSON 导入/导出
- 中英文界面切换

## 快速开始

需要 HTTP 服务器（加载字库 bin 文件需要）：

```bash
cd WebIDE
python -m http.server 8080
```

浏览器打开 http://localhost:8080

## 操作方式

| 按键 | 功能 |
|------|------|
| ↑ / ↓ 或 W / S | 导航 |
| Enter / Space | 确认 |
| Escape / Backspace | 返回 |
| 鼠标滚轮 | 模拟旋转编码器 |

也可以点击界面上的虚拟按钮操作。

## 项目结构

```
WebIDE/
├── index.html          入口页面
├── style.css           暗色主题样式
├── fonts/              GB2312 中文字库
│   ├── HZK12.bin
│   ├── HZK16.bin
│   ├── HZK20.bin
│   └── unicode_gb2312_map.bin
└── js/
    ├── i18n.js         中英文国际化
    ├── fonts.js        ASCII 字模 + 中文字库加载
    ├── oled.js         128×64 缓冲区 + 绘图原语
    ├── animation.js    非线性/PID 动画引擎
    ├── ui-core.js      UI 常量、状态、工具函数
    ├── ui-list.js      LIST 菜单渲染
    ├── ui-tiles.js     TILES + DEPTH 磁贴渲染
    ├── ui-tiles-adv.js HOPE + ARC 磁贴渲染
    ├── ui-main.js      窗口、输入、导航、主循环
    ├── codegen.js      C 代码生成器
    ├── editor.js       菜单树编辑器 + 属性面板
    └── app.js          应用初始化 + 项目管理
```

## 使用流程

1. 左侧面板编辑菜单结构（添加页面、添加菜单项）
2. 中间 Canvas 实时预览 OLED 显示效果
3. 右侧面板调整选中页面/菜单项的属性
4. 点击「生成C代码」导出可编译的 C 结构体代码
5. 将代码粘贴到你的 `OLED_UI_MenuData.c` 中

## 部署

纯静态文件，可直接部署到 GitHub Pages、Vercel 或任何静态托管服务。

## 致谢

- [OLED_UI](https://github.com/bdth-7777777/OLED_UI) - 原始嵌入式 UI 框架
- bilibili @上nm网课呢 - 框架作者
