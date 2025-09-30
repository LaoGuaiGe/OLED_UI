#ifndef _APP_GAME_H_
#define _APP_GAME_H_

typedef struct {
	char x;
	char y;
	char width;
	char height;
}barrier_struct;


void game_init(void);
void game_over(void);
void game_ball_update(void);
void game_barrier_update(void);
void game_barrier_show(void);
void game_ball_show(void);
void game_loop(void);
#endif
