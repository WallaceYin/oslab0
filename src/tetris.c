#include <tetris.h>
#include <ylib.h>

_Tetris_global trs;

static uint32_t bp[SCREEN_HEIGHT][SCREEN_WIDTH]; //black pixel
void screen_clear(void)
{
	draw_rect((uint32_t *)bp, LEFT_BOND, UP_BOND, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void kbd_event(int key)
{
	switch (key)
	{
		case (_KEY_F5): //Game Restart
		screen_clear();
		case (_KEY_F2): //Game Start
		game_init();
		break;

		case (_KEY_F3): //Pause
		break;

		case (_KEY_A): //Left Move
		break;

		case (_KEY_D): //Right Move
		break;

		case (_KEY_W): //Rotate
		break;

		case (_KEY_S): //Down Move
		break;
	}
}

void game_init(void)
{
	memset((void *)(trs.pm), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	memset((void *)(trs.bg), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
}

void game_process(void)
{
	//TODO
}

void screen_update(void)
{
	draw_rect((uint32_t *)trs.pm, LEFT_BOND, UP_BOND, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void line_eliminate(void)
{
	uint32_t *p;
	for (;;)
	{
		p = &(trs.bg[SCREEN_HEIGHT - 1][0]);
		while ((*p != 0x00000000) && (p != &(trs.bg[SCREEN_HEIGHT - 1][SCREEN_WIDTH - 1]) + 1))
			p++;
		if (p != &trs.bg[SCREEN_HEIGHT - 1][SCREEN_WIDTH - 1] + 1)
			break;
		for (int i = SCREEN_HEIGHT - 1; i >= 0 ; i--)
			for (p = &(trs.bg[i][0]); p <= &(trs.bg[i][SCREEN_WIDTH - 1]; p++))
				*p = *(p - SCREEN_WIDTH);
		for (p = &(trs.bg[0][0]); p <= &(trs.bg[0][SCREEN_WIDTH - 1]; p++))
		*p = 0x00000000;
	}
}
