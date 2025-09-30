#include "app_game.h"
#include "time.h"
#include "stdlib.h"
#include "stdio.h"

#include "OLED_driver.h"
#include "OLED.h"
#include "hw_delay.h"
// 显示屏幕宽高
#define SCREEN_HEIGHT  64
#define SCREEN_WIDTH  128

barrier_struct barrier_up;          //顶部障碍物对象
barrier_struct barrier_down;        //底部障碍物对象
barrier_struct ball;                //小球对象

int x = 30, y = 8;	     	        //小球x y位置
float gravity = 1;			        // 重力
float y_speed = 0;			        // y 方向加速度
int barrier_x = SCREEN_WIDTH;	    // 障碍物X轴起始位置
int random_y = 32;			        // 障碍物出口Y位置
int barrier_width = 8;              // 障碍物宽度
int clearance_height = 32;          // 小球可通过的间隙高度
int ball_height = 6;                 // 小球整体高度

// 游戏参数参数化
void game_init(void)
{
    barrier_x = SCREEN_WIDTH;

	ball.x = x;
	ball.y = y;
	ball.height = ball_height;

	barrier_up.x = barrier_x;
	barrier_up.y = 0;
	barrier_up.width = barrier_width;
	barrier_up.height = random_y;

	barrier_down.x = barrier_x;
	barrier_down.y = random_y + clearance_height;
	barrier_down.width = barrier_width;
	barrier_down.height = SCREEN_HEIGHT;
}

// 游戏结束条件判断
void game_over(void)
{
    //碰到底部结束游戏
    if( y >= SCREEN_HEIGHT - (ball.height/2) )
    {
        y = SCREEN_HEIGHT - (ball.height/2);
        y_speed = -y_speed;
        OLED_DrawCircle(ball.x, y, ball.height / 2, OLED_FILLED);
        OLED_Update();
        while(1); 
    }
    //碰到障碍物结束游戏
    //障碍物左边 == 小球的右边
    if( barrier_x == ball.x-(ball.height/2) )
    {
        //小球的顶部小于顶层障碍物出口的顶部(小球碰到顶部障碍物左边)
        //小球的底部大于底层障碍物出口的顶部(小球碰到顶部障碍物左边)
        if( (ball.y-(ball.height/2) < random_y) || (ball.y+(ball.height/2) > random_y + clearance_height) )
        {
            OLED_DrawCircle(ball.x, ball.y, ball.height / 2, OLED_FILLED);
            OLED_Update();
            while(1); 
        }
    }
    //小球在障碍物中间范围
    // 小球终点X坐标在出口初始X坐标右边 并且  出口终点X坐标在小球起始X坐标左边
    if( (ball.x+(ball.height/2) >= barrier_x) && ( barrier_x+barrier_width >= ball.x-(ball.height/2) )  )
    {	
        //小球顶部碰到顶部障碍物的下面 或者 小球底部碰到底部障碍物的上面
        if( (ball.y-(ball.height/2) <= random_y)  || (ball.y+(ball.height/2) > random_y + clearance_height) )
        {
            OLED_DrawCircle(ball.x, ball.y, ball.height / 2, OLED_FILLED);
            OLED_Update();
            while(1); 
        }
    }
}

// 小球坐标更新
void game_ball_update(void)
{
    // 小球下落速度控制
    y_speed = y_speed + gravity;
    y = y + y_speed;
    // 小球碰到顶部反弹
    if( y <= (ball.height/2) )
    {
        y = (ball.height/2);
        y_speed = -y_speed;
    }
}

// 障碍物坐标更新
void game_barrier_update(void)
{
    // 障碍物结束判断
    if( barrier_x <= 1 ) 
    {
        // 清除旧障碍物显示
        OLED_ClearArea(barrier_up.x, 0, barrier_up.width, SCREEN_HEIGHT);
        // 重设障碍物X轴位置
        barrier_x = SCREEN_WIDTH;		
        // 计算随机的障碍物出口
        random_y = rand()%( SCREEN_HEIGHT - clearance_height );  
    }
}

// 障碍物显示
void game_barrier_show(void)
{
    // 障碍物位置刷新
    barrier_up.x = barrier_x;
    barrier_up.y = 0;
    barrier_up.width = barrier_width;
    barrier_up.height = random_y;
    // 消除顶层障碍物残影
    OLED_ClearArea(barrier_up.x+1, barrier_up.y, barrier_up.width, barrier_up.height);
    // 画顶层障碍物
    OLED_DrawRectangle(barrier_up.x,barrier_up.y,barrier_up.width,barrier_up.height,OLED_FILLED); 

    barrier_down.x = barrier_x;
    barrier_down.y = random_y + clearance_height;
    barrier_down.width = barrier_width;
    barrier_down.height = SCREEN_HEIGHT;
    // 消除底层障碍物残影
    OLED_ClearArea(barrier_down.x+1, barrier_down.y, barrier_down.width, barrier_down.height);
    // 画底层障碍物
    OLED_DrawRectangle(barrier_down.x,barrier_down.y,barrier_down.width,barrier_down.height,OLED_FILLED); 
}

// 小球显示
void game_ball_show(void)
{
    //清除小球残影
    OLED_ClearArea(ball.x-(ball.height/2), ball.y-(ball.height/2), ball.height+1, ball.height+1);
    
    //重设小球坐标
    ball.x = x;
    ball.y = y;
    ball.height = ball_height;

    //画小球
    OLED_DrawCircle(ball.x, ball.y, ball.height / 2, OLED_FILLED);
}


void game_loop(void)
{
    static int time_num = 0;
    
    time_num++;
    if( time_num > 12 )
    {
        time_num = 0;
        game_ball_update();
    }

    game_ball_show();

    game_barrier_update();
    game_barrier_show();

    game_over();
    //速度限制
    delay_ms(1);

    //刷屏
    OLED_Update();
}








