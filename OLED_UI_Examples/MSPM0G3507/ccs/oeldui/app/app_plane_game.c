/**
 * app_plane_game.c
 * 飞机大战游戏主实现文件
 */

// 包含必要的头文件
#include "app_plane_game.h"
#include "OLED.h"
#include "hw_delay.h"
#include "stdlib.h"

// =============================================================================
// 全局变量定义
// =============================================================================
static int game_status = PLANE_GAME_READY;       // 当前游戏状态
static plane_game_object_t player;               // 玩家飞机
static plane_game_object_t enemies[MAX_ENEMIES]; // 敌机数组
static plane_game_object_t bullets[MAX_BULLETS]; // 子弹数组
static unsigned int score = 0;                   // 当前分数
static unsigned int score_counter = 0;           // 分数计数器
static unsigned int fire_cooldown = 0;           // 发射冷却计数器
static bool exit_requested = false;              // 退出请求标志

// 游戏输入状态
static plane_game_input_t game_input = {
    .up = false,
    .down = false,
    .left = false,
    .right = false,
    .click = false
};

// =============================================================================
// 内部函数声明
// =============================================================================
static void draw_player(void);
static void draw_enemy(plane_game_object_t *enemy);
static void draw_bullet(plane_game_object_t *bullet);
static void draw_score(void);
static int transform_x(int x, int y);
static int transform_y(int x, int y);

// 坐标转换函数，将游戏坐标转换为屏幕显示坐标
// 我们不需要额外的坐标转换，直接使用原始坐标系统
static int transform_x(int x, int y) {
    return x; // 直接使用x坐标（0-127）
}

static int transform_y(int x, int y) {
    return y; // 直接使用y坐标（0-63）
}

static void draw_player(void) {
    if (!player.active) return;
    
    // 确保在有效显示范围内
    if (player.x < 0 || player.y < 0 || 
        player.x + player.width > SCREEN_WIDTH || 
        player.y + player.height > SCREEN_HEIGHT) {
        return;
    }
    
    int display_x = transform_x(player.x, player.y);
    int display_y = transform_y(player.x, player.y);
    
    // 像素风格飞机绘制（12x10像素）- 前端向右，与子弹发射方向一致
    
    // 绘制飞机的三角形前端（向右）
    OLED_DrawLine(display_x + 11, display_y + 4, display_x + 6, display_y);  // 右上斜线
    OLED_DrawLine(display_x + 11, display_y + 4, display_x + 6, display_y + 9);  // 右下斜线
    OLED_DrawLine(display_x + 6, display_y, display_x + 6, display_y + 9);  // 垂直中线
    
    // 绘制飞机的主体
    for (int i = 0; i < 5; i++) {  // 5行主体区域
        OLED_DrawLine(display_x + 3, display_y + 2 + i, display_x + 6, display_y + 2 + i);
    }
    
    // 绘制飞机的机翼
    OLED_DrawLine(display_x + 4, display_y + 1, display_x + 0, display_y - 1);  // 左上机翼
    OLED_DrawLine(display_x + 0, display_y - 1, display_x + 0, display_y + 0);  // 左上机翼末端
    
    OLED_DrawLine(display_x + 4, display_y + 8, display_x + 0, display_y + 10);  // 右下机翼
    OLED_DrawLine(display_x + 0, display_y + 9, display_x + 0, display_y + 10);  // 右下机翼末端
    
    // 绘制飞机的尾部细节（在左侧）
    OLED_DrawLine(display_x + 3, display_y + 4, display_x + 2, display_y + 3);  // 尾部装饰1
    OLED_DrawLine(display_x + 3, display_y + 5, display_x + 2, display_y + 6);  // 尾部装饰2
    
    // 绘制飞机的驾驶舱（一个小点，位于飞机中部偏右）
    if (display_x + 7 >= 0 && display_y + 4 >= 0) {
        OLED_DrawPoint(display_x + 7, display_y + 4);
    }
}

/**
 * 绘制敌机
 */
static void draw_enemy(plane_game_object_t *enemy) {
    if (enemy->active) {
        // 使用坐标转换绘制竖屏模式下的敌机主体（三角形）
        int x1 = transform_x(enemy->x + enemy->width/2, enemy->y);
        int y1 = transform_y(enemy->x + enemy->width/2, enemy->y);
        int x2 = transform_x(enemy->x, enemy->y + enemy->height);
        int y2 = transform_y(enemy->x, enemy->y + enemy->height);
        int x3 = transform_x(enemy->x + enemy->width, enemy->y + enemy->height);
        int y3 = transform_y(enemy->x + enemy->width, enemy->y + enemy->height);
        
        OLED_DrawLine(x1, y1, x2, y2);
        OLED_DrawLine(x1, y1, x3, y3);
        OLED_DrawLine(x2, y2, x3, y3);
    }
}

/**
 * 绘制子弹
 */
static void draw_bullet(plane_game_object_t *bullet) {
    if (bullet->active) {
        // 使用坐标转换绘制竖屏模式下的子弹（小矩形）
        int display_x = transform_x(bullet->x, bullet->y);
        int display_y = transform_y(bullet->x, bullet->y);
        OLED_DrawRectangle(display_x, display_y, bullet->height, bullet->width, OLED_FILLED);
    }
}

/**
 * 绘制分数
 */
static void draw_score(void) {
    // 显示当前分数在右上角
    OLED_ShowString(10, 5, "Score", OLED_6X8_HALF);
    OLED_ShowNum(40, 5, score, 3, OLED_6X8_HALF);
}

// =============================================================================
// 公共接口函数实现
// =============================================================================

/**
 * 设置单击信号函数
 */
void plane_game_set_click(void) {
    game_input.click = true;
}

/**
 * 设置方向键信号函数
 */
// 向上移动（竖屏模式）
void plane_game_set_up(void) {
    game_input.up = true;
    if (player.active && player.y > 0) {
        player.y -= player.speed;
        if (player.y < 0) player.y = 0;
    }
}

// 向下移动（竖屏模式）
void plane_game_set_down(void) {
    game_input.down = true;
    if (player.active && player.y < SCREEN_HEIGHT - player.height) {
        player.y += player.speed;
        if (player.y > SCREEN_HEIGHT - player.height) {
            player.y = SCREEN_HEIGHT - player.height;
        }
    }
}
/**
 * 清除所有方向键信号
 */
void plane_game_clear_controls(void) {
    game_input.up = false;
    game_input.down = false;
    game_input.left = false;
    game_input.right = false;
    game_input.click = false;
}

/**
 * 游戏参数初始化
 */
void plane_game_init(void) {
    // 初始化游戏状态
    game_status = PLANE_GAME_READY;
    score = 0;
    score_counter = 0;
    fire_cooldown = 0;
    exit_requested = false;
    plane_game_clear_controls();
    
    // 初始化玩家飞机（标准128x64屏幕，左侧居中）
    player.x = PLAYER_INIT_X;  // 左侧位置
    player.y = SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2;    // 垂直居中位置（0-63范围内）
    player.width = PLAYER_WIDTH;
    player.height = PLAYER_HEIGHT;
    player.speed = PLAYER_SPEED; // 使用常量定义的速度值
    player.active = true;
    
    // 初始化敌机数组
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
    }
    
    // 初始化子弹数组
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    
    // 清除屏幕并显示游戏标题 - 标准128x64 OLED
    OLED_Clear();
    OLED_ShowString(45, 20, "PLANE", OLED_6X8_HALF);
    OLED_ShowString(40, 30, "GAME", OLED_6X8_HALF);
    OLED_ShowString(30, 40, "Click to start", OLED_6X8_HALF);
    OLED_ShowString(25, 50, "UD to move", OLED_6X8_HALF);
    OLED_Update();
    plane_game_clear_controls();
}

/**
 * 处理按键输入
 */
void plane_game_handle_input(void) {
    // 处理游戏状态转换
    if (game_status == PLANE_GAME_READY) {
        // 准备状态：按任意键开始游戏
        if (game_input.up || game_input.down) {
            game_status = PLANE_GAME_RUNNING;
        }
    } else if (game_status == PLANE_GAME_RUNNING) {
        // 运行状态：处理移动和射击
        // 移动逻辑在update函数中处理
        
        // 自动发射子弹（不需要手动控制）
    } else if (game_status == PLANE_GAME_OVER) {
        // 游戏结束状态：按任意键重新开始
        if (game_input.up || game_input.down) {
            plane_game_init();
        }
    }
    
    // 使用统一的函数清除所有输入状态
        plane_game_clear_controls();
}

/**
 * 更新游戏状态
 */
void plane_game_update(void) {
    if (game_status != PLANE_GAME_RUNNING) return;
    
    // 更新分数
    score_counter++;
    
    // 更新分数（每30帧增加1分）
    if (score_counter >= 30) {
        score++;
        score_counter = 0;
    }
    
    // 更新发射冷却
    if (fire_cooldown > 0) {
        fire_cooldown--;
    }
    
    // 自动发射子弹
    if (fire_cooldown == 0 && player.active) {
        plane_game_fire_bullet();
        fire_cooldown = FIRE_INTERVAL;
    }
    
    // 生成敌机（从右侧生成）
    // if (rand() % 2 == 0) {  // 增加敌机生成频率
        plane_game_generate_enemy();
    // }
    
    // 更新敌机位置 - 标准128x64屏幕：敌机从右向左移动（x从127向0移动）
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].x -= enemies[i].speed;  // 敌机向左移动
            
            // 检查敌机是否移出屏幕左侧
            if ((enemies[i].x <= 0) || (enemies[i].x>=60000) ) {
                enemies[i].active = false;
            }
        }
    }
    
    // 更新子弹位置 - 标准128x64屏幕：子弹从左向右移动（x从0向127移动）
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].x += bullets[i].speed;  // 子弹向右移动
            
            // 检查子弹是否移出屏幕右侧
            if (bullets[i].x > SCREEN_WIDTH) {
                bullets[i].active = false;
            }
        }
    }
    
    // 碰撞检测（游戏逻辑保持不变）
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (plane_game_check_collision(&bullets[i], &enemies[j])) {
                        // 子弹和敌机碰撞，两者都失效
                        bullets[i].active = false;
                        enemies[j].active = false;
                        score += 10;  // 击中敌机加分
                    }
                }
            }
        }
    }
    
    // 检测玩家和敌机的碰撞
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (plane_game_check_collision(&player, &enemies[i])) {
                plane_game_over();
                return;
            }
        }
    }
}

/**
 * 渲染游戏画面
 */
void plane_game_render(void) {

    if (game_status == PLANE_GAME_RUNNING) {

        // 清空屏幕
        OLED_Clear();

        // 绘制游戏元素
        draw_player();
        
        // 绘制所有激活的敌机
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                draw_enemy(&enemies[i]);
            }
        }
        
        // 绘制所有激活的子弹
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                draw_bullet(&bullets[i]);
            }
        }
        
        // 绘制分数
        draw_score();
    } else if (game_status == PLANE_GAME_OVER) {

        // 清空屏幕
        OLED_Clear();

        // 游戏结束画面 - 标准128x64 OLED
        OLED_DrawRectangle(20, 10, 88, 44, OLED_UNFILLED);
        OLED_ShowString(35, 20, "GAME", OLED_6X8_HALF);
        OLED_ShowString(35, 30, "OVER", OLED_6X8_HALF);
        OLED_ShowString(30, 40, "Click to retry", OLED_6X8_HALF);
        
        // 显示最终分数
        OLED_ShowString(40, 50, "Score:", OLED_6X8_HALF);
        OLED_ShowNum(65, 50, score, 3, OLED_6X8_HALF);
    }
    
    // 更新OLED显示
    OLED_Update();
}

/**
 * 碰撞检测
 */
bool plane_game_check_collision(plane_game_object_t *obj1, plane_game_object_t *obj2) {
    // 矩形碰撞检测核心逻辑（游戏逻辑中的碰撞检测，不涉及显示坐标转换）
    bool x_overlap = (obj1->x < obj2->x + obj2->width) && 
                   (obj1->x + obj1->width > obj2->x);
    bool y_overlap = (obj1->y < obj2->y + obj2->height) && 
                   (obj1->y + obj1->height > obj2->y);
    
    return x_overlap && y_overlap;
}

/**
 * 生成敌机
 */
void plane_game_generate_enemy(void) {
    // 找到一个未激活的敌机
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            // 初始化敌机属性 - 从右侧生成（x=128，y在0-63范围内）
            enemies[i].x = SCREEN_WIDTH;  // 屏幕外右侧（128）
            enemies[i].y = rand() % (SCREEN_HEIGHT - ENEMY_HEIGHT);  // 随机垂直位置（0-63范围内）
            enemies[i].width = ENEMY_WIDTH;
            enemies[i].height = ENEMY_HEIGHT;
            enemies[i].speed = ENEMY_SPEED_MIN + (rand() % (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN + 1));
            enemies[i].active = true;
            break;
        }
    }
}

/**
 * 发射子弹
 */
void plane_game_fire_bullet(void) {
    // 找到一个未激活的子弹
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            // 初始化子弹属性（从飞机右侧发射）
            bullets[i].x = player.x + player.width;
            bullets[i].y = player.y + player.height / 2 - BULLET_HEIGHT / 2;
            bullets[i].width = BULLET_WIDTH;
            bullets[i].height = BULLET_HEIGHT;
            bullets[i].speed = BULLET_SPEED;
            bullets[i].active = true;
            break;
        }
    }
}

/**
 * 游戏结束处理
 */
void plane_game_over(void) {
    game_status = PLANE_GAME_OVER;
}

/**
 * 游戏主循环
 */
void plane_game_loop(void) {
    plane_game_init();
    
    // 游戏主循环
    while (1) {
        plane_game_handle_input();  // 处理输入状态转换
        plane_game_update();       // 更新游戏状态（包括移动）
        plane_game_render();       // 渲染画面
        
        // 每个帧结束后清除方向键状态，这样需要持续按住才能保持移动
        plane_game_clear_controls();
        
        // 检查是否有退出请求
        if (plane_game_should_exit()) {
            OLED_Clear();    // 清除屏幕
            break;          // 跳出游戏主循环，返回上一级菜单
        }
        
        delay_ms(30);  // 控制游戏帧率约33FPS
    }
}

/**
 * 设置游戏退出标志
 */
void plane_game_request_exit(void) {
    exit_requested = true;
}

/**
 * 检查是否有退出请求
 */
bool plane_game_should_exit(void) {
    return exit_requested;
}