/**
 * app_dino_game.c
 * 小恐龙游戏主实现文件
 */

// 包含必要的头文件
#include "app_dino_game.h"
#include "OLED.h"
#include "hw_delay.h"
#include "stdlib.h"

// =============================================================================
// 游戏常量定义
// =============================================================================
#define SCREEN_WIDTH    128    // 屏幕宽度（像素）
#define SCREEN_HEIGHT   64     // 屏幕高度（像素）
#define GROUND_Y        50     // 地面Y坐标位置

// 恐龙参数设置
#define DINO_WIDTH      8      // 恐龙宽度（像素）
#define DINO_HEIGHT     10     // 恐龙高度（像素）
#define DINO_JUMP_SPEED 4      // 恐龙跳跃初始速度
#define DINO_X          20     // 恐龙初始X坐标位置

// 障碍物参数设置
#define OBSTACLE_WIDTH   6     // 障碍物宽度（像素）
#define OBSTACLE_HEIGHT1 12    // 小仙人掌高度（像素）
#define OBSTACLE_HEIGHT2 18    // 大仙人掌高度（像素）
#define OBSTACLE_SPEED   2     // 障碍物基础移动速度

// 游戏状态定义
#define GAME_READY     0       // 游戏准备状态
#define GAME_RUNNING   1       // 游戏运行状态
#define GAME_OVER      2       // 游戏结束状态

// =============================================================================
// 全局变量定义
// =============================================================================
dino_state_t dino_state = DINO_RUNNING;
dino_game_object_t dino;
dino_game_object_t obstacles[3];
int game_status = GAME_READY;
int score = 0;
int score_counter = 0;
int speed_level = 1;
float dino_gravity = 0.3;
float jump_velocity = 0;
int animation_frame = 0;
int ground_offset = 0;
bool exit_requested = false;  // 退出请求标志，由外部设置

// =============================================================================
// 游戏输入接口定义
// =============================================================================

// 游戏输入接口
typedef struct {
    bool click;  // 单击标志
} game_input_t;

game_input_t game_input = {0};

// =============================================================================
// 绘制函数
// =============================================================================

// 绘制恐龙
void draw_dino(int x, int y, dino_state_t state) {
    // 清除之前的恐龙图像区域
    OLED_ClearArea(x-1, y-1, DINO_WIDTH+2, DINO_HEIGHT+2);
    
    if (state == DINO_DEAD) {
        // 死亡状态的恐龙
        OLED_DrawRectangle(x, y+8, 2, 8, OLED_FILLED);   // 腿部
        OLED_DrawRectangle(x+2, y+6, 2, 10, OLED_FILLED); // 腿部
        OLED_DrawRectangle(x+4, y, 8, 10, OLED_FILLED);   // 身体
        OLED_DrawRectangle(x+10, y-4, 4, 6, OLED_FILLED); // 头部
    } else if (state == DINO_DUCKING) {
        // 下蹲状态的恐龙
        OLED_DrawRectangle(x, y+4, 12, 10, OLED_FILLED);  // 身体
        OLED_DrawRectangle(x+10, y, 4, 6, OLED_FILLED);   // 头部
    } else {
        // 跑步或跳跃状态的恐龙
        // 根据动画帧切换腿部姿势，实现跑步动画
        if (animation_frame % 10 < 5) {
            // 跑步帧1
            OLED_DrawRectangle(x, y+8, 2, 8, OLED_FILLED);   // 左腿
            OLED_DrawRectangle(x+4, y+12, 2, 4, OLED_FILLED); // 右腿
        } else {
            // 跑步帧2
            OLED_DrawRectangle(x, y+12, 2, 4, OLED_FILLED);   // 左腿
            OLED_DrawRectangle(x+4, y+8, 2, 8, OLED_FILLED);  // 右腿
        }
        OLED_DrawRectangle(x+4, y, 8, 10, OLED_FILLED);   // 身体
        OLED_DrawRectangle(x+10, y-4, 4, 6, OLED_FILLED); // 头部
    }
}

// 绘制障碍物（仙人掌）
void draw_obstacle(dino_game_object_t *obs) {
    if (obs->width == 0 || obs->height == 0) return;
    
    // 清除之前的障碍物图像
    OLED_ClearArea(obs->x-1, obs->y-1, obs->width+2, obs->height+2);
    
    // 绘制仙人掌主体
    OLED_DrawRectangle(obs->x, obs->y, obs->width, obs->height, OLED_FILLED);
    
    // 根据障碍物高度添加仙人掌的刺
    if (obs->height == OBSTACLE_HEIGHT1) {
        // 小仙人掌刺
        OLED_DrawRectangle(obs->x-2, obs->y+4, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x+obs->width, obs->y+4, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x-2, obs->y+10, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x+obs->width, obs->y+10, 2, 2, OLED_FILLED);
    } else {
        // 大仙人掌刺
        OLED_DrawRectangle(obs->x-2, obs->y+4, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x+obs->width, obs->y+4, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x-2, obs->y+12, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x+obs->width, obs->y+12, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x-2, obs->y+20, 2, 2, OLED_FILLED);
        OLED_DrawRectangle(obs->x+obs->width, obs->y+20, 2, 2, OLED_FILLED);
    }
}

// 绘制地面
void draw_ground() {
    // 绘制地面主线
    OLED_DrawLine(0, GROUND_Y, SCREEN_WIDTH-1, GROUND_Y);
    
    // 绘制地面花纹
    for (int x = -ground_offset % 4; x < SCREEN_WIDTH; x += 4) {
        if (x >= 0) {
            OLED_DrawLine(x, GROUND_Y + 1, x + 1, GROUND_Y + 1); 
        }
    }
}

// 绘制分数
void draw_score() {
    // 显示当前分数在右上角
    // 计算适合右上角的位置，确保文本不会超出屏幕
    // unsigned int score_str_width = 30; // "Score:"字符串宽度 (6像素宽字体 * 5个字符)
    // unsigned int num_width = 18;       // 数字宽度 (6像素宽字体 * 3位数)
    // unsigned int total_width = 50; // 加2个像素的间距
    // unsigned int start_x = SCREEN_WIDTH - total_width; // 从右边缘计算起始位置
    OLED_ShowString(78, 5, "Score", OLED_6X8_HALF);
    OLED_ShowNum(110, 5, score, 3, OLED_6X8_HALF);
}

// =============================================================================
// 游戏核心函数
// =============================================================================

// 初始化游戏
void dino_game_init() {
    // 初始化随机数生成器
    srand(animation_frame);
    
    // 初始化恐龙
    dino.x = DINO_X;
    dino.y = GROUND_Y - DINO_HEIGHT;
    dino.width = DINO_WIDTH;
    dino.height = DINO_HEIGHT;
    dino_state = DINO_RUNNING;
    jump_velocity = 0;
    
    // 初始化障碍物
    for (int i = 0; i < 3; i++) {
        obstacles[i].x = 0;
        obstacles[i].y = 0;
        obstacles[i].width = 0;  // 初始为无效状态
        obstacles[i].height = 0;
        obstacles[i].speed = OBSTACLE_SPEED;
    }
    
    // 初始化游戏状态
    game_status = GAME_READY;
    score = 0;
    score_counter = 0;
    speed_level = 1;
    animation_frame = 0;
    ground_offset = 0;
    exit_requested = false;  // 重置退出请求标志
    
    // 清空屏幕并显示游戏标题
    OLED_Clear();
    OLED_ShowString(35, 20, "DINO GAME", OLED_7X12_HALF);
    OLED_ShowString(25, 40, "Press UP to jump", OLED_6X8_HALF);
    OLED_ShowString(10, 55, "Try to jump over cacti", OLED_6X8_HALF);
    OLED_Update();
}

// 处理按键输入
void dino_game_handle_input(void) {
    if (game_status == GAME_READY) {
        // 准备状态下，单击开始游戏
        if (game_input.click) {
            game_input.click = 0;
            game_status = GAME_RUNNING;
            OLED_Clear();
        }
    } else if (game_status == GAME_RUNNING) {
        // 运行状态下，单击让恐龙跳跃
        if (game_input.click && dino_state != DINO_JUMPING && dino.y >= GROUND_Y - DINO_HEIGHT) {
            game_input.click = 0;
            dino_state = DINO_JUMPING;
            jump_velocity = -DINO_JUMP_SPEED;
        }
    } else if (game_status == GAME_OVER) {
        // 游戏结束状态下，单击重新开始游戏
        if (game_input.click) {
            game_input.click = 0;
            dino_game_init();
        }
    }
}

// 更新游戏状态
void dino_game_update(void) {
    if (game_status != GAME_RUNNING) return;
    
    // 更新动画帧计数器
    animation_frame++;
    
    // 更新地面滚动 - 移到update函数中统一管理
    int ground_speed = 1 + (speed_level - 1) / 3;
    ground_offset += ground_speed;
    if (ground_offset >= SCREEN_WIDTH) {
        ground_offset = 0;
    }
    
    // 更新分数 - 每10帧增加1分
    score_counter++;
    if (score_counter >= 10) {
        score_counter = 0;
        score++;
        
        // 每100分增加一次难度，但最大难度为5
        if (score % 100 == 0 && speed_level < 5) {
            speed_level++;
            for (int i = 0; i < 3; i++) {
                obstacles[i].speed = OBSTACLE_SPEED + (speed_level - 1) / 2;
            }
        }
    }
    
    // 更新恐龙状态 - 处理跳跃物理
    if (dino_state == DINO_JUMPING) {
        // 应用重力加速度
        jump_velocity += dino_gravity;
        // 更新恐龙的Y坐标
        dino.y += jump_velocity;
        
        // 限制恐龙最大跳跃高度
        if (dino.y <= 5) {  // 距离屏幕顶部至少保留5个像素
            dino.y = 5;
            if (jump_velocity < 0) {
                jump_velocity = 0.1;  // 给一个小的正速度开始下落
            }
        }
        
        // 检测是否落地
        if (dino.y >= GROUND_Y - DINO_HEIGHT) {
            dino.y = GROUND_Y - DINO_HEIGHT;
            dino_state = DINO_RUNNING;
            jump_velocity = 0;
        }
    }
    
    // 障碍物生成计时器
    static int obstacle_spawn_timer = 0;
    
    // 更新障碍物位置
    for (int i = 0; i < 3; i++) {
        if (obstacles[i].x > 0) {  // 只更新激活的障碍物
            obstacles[i].x -= obstacles[i].speed;
            
            // 障碍物完全移出屏幕左侧后重置
            if (obstacles[i].x <= 0) {
                obstacles[i].x = 0;
                obstacles[i].width = 0;
                obstacles[i].height = 0;
            }
        }
    }
    
    // 更新障碍物生成计时器
    obstacle_spawn_timer++;
    
    // 计算当前活跃障碍物数量
    int active_obstacles = 0;
    for (int i = 0; i < 3; i++) {
        if (obstacles[i].x > 0) active_obstacles++;
    }
    
    // 优化障碍物生成逻辑：减少生成间隔，允许更多障碍物
    int spawn_interval = 100 - speed_level * 10;  // 减少初始间隔并增加难度影响
    spawn_interval = (spawn_interval < 50) ? 50 : spawn_interval;  // 最小间隔设为50帧（约1.5秒）
    
    // 修改生成条件：允许同时存在最多3个障碍物，且考虑前一个障碍物的距离
    bool should_spawn = (obstacle_spawn_timer >= spawn_interval && active_obstacles < 3);
    
    // 如果有障碍物，确保与前一个障碍物有足够的距离
    if (should_spawn && active_obstacles > 0) {
        // 查找最右侧的障碍物
        int rightmost_x = -1;
        for (int i = 0; i < 3; i++) {
            if (obstacles[i].x > 0 && obstacles[i].x > rightmost_x) {
                rightmost_x = obstacles[i].x;
            }
        }
        
        // 确保与前一个障碍物有足够的距离
        int min_distance = 40 + speed_level * 5;  // 距离随速度调整
        should_spawn = (rightmost_x < SCREEN_WIDTH - min_distance);
    }
    
    if (should_spawn) {
        // 找到一个空闲的障碍物槽并生成
        for (int i = 0; i < 3; i++) {
            if (obstacles[i].width == 0) {
                bool is_big = (rand() % 4 == 0);
                
                // 设置障碍物属性
                obstacles[i].x = SCREEN_WIDTH + 10;
                obstacles[i].y = GROUND_Y - (is_big ? OBSTACLE_HEIGHT2 : OBSTACLE_HEIGHT1);
                obstacles[i].width = OBSTACLE_WIDTH;
                obstacles[i].height = is_big ? OBSTACLE_HEIGHT2 : OBSTACLE_HEIGHT1;
                // 增加速度增长，使游戏更有挑战性
                obstacles[i].speed = OBSTACLE_SPEED + (speed_level - 1) / 2;
                
                // 重置计时器
                obstacle_spawn_timer = 0;
                break;
            }
        }
    }
    
    // 检查碰撞
    if (dino_game_check_collision()) {
        dino_state = DINO_DEAD;
        game_status = GAME_OVER;
    }
}

// 渲染游戏画面
void dino_game_render(void) {
    if (game_status != GAME_RUNNING && game_status != GAME_OVER) return;
    
    // 清空屏幕
    OLED_Clear();
    
    // 绘制游戏元素
    draw_ground();
    draw_dino(dino.x, dino.y, dino_state);
    
    // 绘制所有激活的障碍物
    for (int i = 0; i < 3; i++) {
        if (obstacles[i].width > 0) {
            draw_obstacle(&obstacles[i]);
        }
    }
    
    // 绘制分数
    draw_score();
    
    // 如果游戏结束，显示结束信息
    if (game_status == GAME_OVER) {
        OLED_DrawRectangle(20, 20, 88, 24, OLED_UNFILLED);
        OLED_ShowString(40, 24, "GAME OVER", OLED_6X8_HALF);
        OLED_ShowString(15, 38, "Press UP to retry", OLED_6X8_HALF);
    }
    
    // 更新OLED显示
    OLED_Update();
}

// 检测碰撞
bool dino_game_check_collision(void) {
    for (int i = 0; i < 3; i++) {
        if (obstacles[i].width > 0) {  // 只检查激活的障碍物
            // 矩形碰撞检测核心逻辑
            bool x_overlap = (dino.x < obstacles[i].x + obstacles[i].width) && 
                           (dino.x + dino.width > obstacles[i].x);
            bool y_overlap = (dino.y < obstacles[i].y + obstacles[i].height) && 
                           (dino.y + dino.height > obstacles[i].y);
            
            if (x_overlap && y_overlap) {
                return true;  // 检测到碰撞
            }
        }
    }
    return false;  // 没有检测到碰撞
}

// 实现生成障碍物函数，避免编译错误
void dino_game_generate_obstacle(void) {
    // 此功能已整合到dino_game_update中
    // 保留空实现以避免编译错误
}


// 游戏结束处理
void dino_game_over(void) {
    game_status = GAME_OVER;
    dino_state = DINO_DEAD;
}

/**
 * 设置游戏退出标志
 * 外部模块可以调用此函数来请求游戏退出
 * 
 * 此函数为外部硬件对接提供接口，当检测到长按事件时，
 * 可以调用此函数通知游戏主循环退出
 */
void dino_game_request_exit(void) {
    exit_requested = true;
}

/**
 * 检查是否有退出请求
 * @return 如果有退出请求返回true，否则返回false
 * 
 * 游戏主循环会定期调用此函数检查是否需要退出
 */
bool dino_game_should_exit(void) {
    return exit_requested;
}

// =============================================================================
// 公共接口函数
// =============================================================================

// 设置单击信号函数
void dino_game_set_click(void) {
    game_input.click = 1;
}

// 游戏主循环
void dino_game_loop(void) {
    dino_game_init();
    
    // 游戏主循环
    while (1) {
        dino_game_handle_input();
        dino_game_update();
        dino_game_render();
        
        // 检查是否有退出请求
        if (dino_game_should_exit()) {
            OLED_Clear();  // 清除屏幕
            break;         // 跳出游戏主循环，返回上一级菜单
        }
        
        // 控制游戏速度，30ms延时控制帧率
        delay_ms(30);
    }
}