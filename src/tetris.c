#include <tetris.h>
#include <ylib.h>

_Tetris_global trs;
_Piece *piece;

static uint32_t bp[SCREEN_HEIGHT][SCREEN_WIDTH]; //black pixel
void screen_clear(void)
{
	draw_rect((uint32_t *)bp, LEFT_BOND, UP_BOND, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void game_block(void)
{
	trs.status = 0;
}

void kbd_event(int key)
{
	switch (key)
	{
		case (_KEY_F5): //Game Restart
			if (!trs.status)
				break;
			screen_clear();
		case (_KEY_F2): //Game Start
			trs.status = 1;
			game_init();
			break;

		case (_KEY_F3): //Pause
			trs.status = 0;
			break;

		case (_KEY_A): //Left Move
			//TODO
			break;

		case (_KEY_D): //Right Move
			//TODO
			break;

		case (_KEY_W): //Rotate
			//TODO
			break;

		case (_KEY_S): //Down Move
			//TODO
			break;
	}
}

void game_init(void)
{
	memset((void *)(trs.pm), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	memset((void *)(trs.bg), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	new_piece(piece);
}

unsigned int time_record = 0xffffffff;
void game_process(void)
{
	if (time_record == 0xffffffff)
		time_record = (unsigned int)up_time();
	else
	{
		if (up_time() < time_record + 1000)
			return;
		time_record = (unsigned int)up_time();
		if (!trs.status)
			return;
		if (bottom_hit(piece))
		{
			add_bg(piece);
			new_piece(piece);
		}
		else
			piece_move(piece, DOWN);
	}
}

void screen_update(void)
{
	//TODO: combine Piece and background
	draw_piece(piece);
	for (int j = 0; j < SCREEN_HEIGHT; j++)
		for (int i = 0; i < SCREEN_WIDTH; i++)
			if (trs.pm[j][i] == 0x00000000)
				trs.pm[j][i] = bg[j][i];
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
			for (p = &(trs.bg[i][0]); p <= &(trs.bg[i][SCREEN_WIDTH - 1]); p++)
				*p = *(p - SCREEN_WIDTH);
		for (p = &(trs.bg[0][0]); p <= &(trs.bg[0][SCREEN_WIDTH - 1]); p++)
		*p = 0x00000000;
	}
}

void draw_piece(_Piece *piece)
{
	int w = piece->w;
	int h = piece->h;
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++)
			if (*(piece->pixel + j * w + i) != 0x00000000) trs.pm[piece->y + j][piece->x + i] = *(piece->pixel + j * w + i);
}

void piece_move(_Piece *piece, int direction)
{
	switch (direction)
	{
		case (STILL):
			break;

		case (LEFT):
			if (piece->x - MIN_DIST >= 0)
				piece->x -= MIN_DIST;
			break;
		
		case (RIGHT):
			if (piece->x + piece->w + MIN_DIST <= SCREEN_WIDTH)
				piece->x += MIN_DIST;
			break;

		case (DOWN):
			if (piece->y + piece->h + MIN_DIST <= SCREEN_HEIGHT)
				piece->y += MIN_DIST;
	}
}

void add_bg(_Piece *piece)
{
	//TODO
}

void new_piece(_Piece *piece)
{
	//TODO
}

int bottom_hit(_Piece *piece)
{
	if (y + MIN_DIST >= SCREEN_HEIGHT)
		return 1;
	for (int y = piece->y; y < piece->y + piece->h; y++)
		for (int x = piece->x; x < piece->x + piece->w; x++)
			if ((*(piece->pixel + (y - piece->y) * piece->w + x - piece->x) != 0x00000000) && (trs.bg[y + MIN_DIST][x] != 0x00000000))
				return 1;
	return 0;
}
