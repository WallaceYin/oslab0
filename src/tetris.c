#include <tetris.h>
#include <ylib.h>

#define DEBUG

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
			if (trs.status)
				piece_move(piece, LEFT);
			break;

		case (_KEY_D): //Right Move
			if (trs.status)
				piece_move(piece, RIGHT);
			break;

		case (_KEY_W): //Rotate
			if (trs.status)
				piece_rotate(piece);
			break;

		case (_KEY_S): //Down Move
			if (trs.status && !bottom_hit(piece))
				piece_move(piece, DOWN);
			break;
	}
}

void game_init(void)
{
	memset((void *)(trs.pm), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	memset((void *)(trs.bg), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	plist_init();
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
	memset((void *)(trs.pm), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	draw_piece(piece);
	for (int j = 0; j < SCREEN_HEIGHT; j++)
		for (int i = 0; i < SCREEN_WIDTH; i++)
			if (trs.pm[j][i] == 0x00000000)
				trs.pm[j][i] = trs.bg[j][i];
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
#ifdef DEBUG
	printf("Move.\n");
#endif
	int movable;
	int x = piece->x;
	int y = piece->y;
	int w = piece->w;
	int h = piece->h;
	uint32_t *pixel = piece->pixel;
	switch (direction)
	{
		case (STILL):
			break;

		case (LEFT):
			movable = 1;
			if (x < MIN_DIST)
				movable = 0;
			else
			{
				for (int j = 0; j < h; j++)
					for (int i = 0; i < w; i++)
						if (*(pixel + j * w + i) != 0 && trs.bg[y + j][x + i - MIN_DIST] != 0)
							movable = 0;
			}
			if (movable)
				piece->x -= MIN_DIST;
			break;
		
		case (RIGHT):
			movable = 1;
			for (int j = 0; j < h; j++)
				for (int i = 0; i < w; i++)
				{
					if (*(pixel + j * w + i) != 0 && x + i + MIN_DIST < SCREEN_WIDTH && trs.bg[y + j][x + i + MIN_DIST] != 0)
						movable = 0;
					else if (*(pixel + j * w + i) != 0 && x + i + MIN_DIST > SCREEN_WIDTH)
						movable = 0;
				}
			if (movable)
				piece->x += MIN_DIST;
			break;

		case (DOWN):
			movable = 1;
			for (int j = 0; j < h; j++)
				for (int i = 0; i < w; i++)
				{	
					if (*(pixel + j * w + i) != 0 && y + j + MIN_DIST < SCREEN_WIDTH && trs.bg[y + j + MIN_DIST][x + i] != 0)
						movable = 0;
					else if (*(pixel + j * w + i) != 0 && y + j + MIN_DIST > SCREEN_HEIGHT)
						movable = 0;
				}
			if (movable)
				piece->y += MIN_DIST;
	}
}

void add_bg(_Piece *piece)
{
#ifdef DEBUG
	printf("Locked in the Background.\n");
#endif
	int x = piece->x;
	int y = piece->y;
	int w = piece->w;
	int h = piece->h;
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++)
			if (*(piece->pixel + j * w + i) != 0x00000000) trs.bg[y + j][x + i] = *(piece->pixel + j * w + i);
}

void new_piece(_Piece *piece)
{
#ifdef DEBUG
	printf("New Piece generated.\n");
#endif
	srand(raw_time());
	int Rand = rand() % 40;
	int posrand = Rand % 8;
	int typerand = Rand % 5;
	piece->x = MIN_DIST * posrand;
	piece->y = 0;
	piece->w = pl[typerand].W * MIN_DIST;
	piece->h = pl[typerand].H * MIN_DIST;
	if (piece->x + piece->w >= SCREEN_WIDTH)
		piece->x = SCREEN_WIDTH - MIN_DIST - piece->w;
	piece->pixel = pl[typerand].pixel;
}

int bottom_hit(_Piece *piece)
{
	for (int y = piece->y; y < piece->y + piece->h; y++)
		for (int x = piece->x; x < piece->x + piece->w; x++)
		{
		if (y + MIN_DIST >= SCREEN_HEIGHT && *(piece->pixel + (y - piece->y) * piece->w + x - piece->x) == 0x00000000)
			continue;
		if ((*(piece->pixel + (y - piece->y) * piece->w + x - piece->x) != 0x00000000) && (trs.bg[y + MIN_DIST][x] != 0x00000000))
		{
#ifdef DEBUG
				printf("Hit bottom.\n");
#endif
				return 1;
		}
		}
	return 0;
}

void plist_init(void)
{
	//piecelist #1
	for (int j = 0; j < MIN_DIST * 2; j++)
		for (int i = 0; i < MIN_DIST * 2; i++)
			_pl_0[j][i] = 0x00006a4a;
	//piecelist #2
	for (int j = 0; j < MIN_DIST * 3; j++)
		for (int i = 0; i < MIN_DIST * 3; i++)
		{
			if (i < 2 * MIN_DIST && j < MIN_DIST)
				_pl_1[j][i] = 0x004f3a00;
			else if (i >= MIN_DIST && j >= MIN_DIST && j < MIN_DIST * 2 )
				_pl_1[j][i] = 0x004f3a00;
			else
				_pl_1[j][i] = 0x00000000;
		}
	//piecelist #3
	for (int j = 0; j < MIN_DIST * 2; j++)
		for (int i = 0; i < MIN_DIST * 2; i++)
		{
			if (j >= MIN_DIST && i < MIN_DIST)
				_pl_2[j][i] = 0x00000000;
			else
				_pl_2[j][i] = 0x006a0020;
		}
	//piecelist #4
	for (int j = 0; j < MIN_DIST * 3; j++)
		for (int i = 0; i < MIN_DIST * 3; i++)
		{
			if (j >= MIN_DIST && j < MIN_DIST * 2)
				_pl_3[j][i] = 0x003f4b00;
			else
				_pl_3[j][i] = 0x00000000;
		}
	//piecelist #5
	for (int j = 0; j < MIN_DIST * 3; j++)
		for (int i = 0; i < MIN_DIST * 3; i++)
		{
			if (j >= MIN_DIST && j < MIN_DIST * 2)
				_pl_4[j][i] = 0x00003e20;
			else if (j < MIN_DIST && i >= MIN_DIST && i < MIN_DIST * 2)
				_pl_4[j][i] = 0x00003e20;
			else
				_pl_4[j][i] = 0x00000000;
		}
	
	pl[0].W = 2; pl[0].H = 2; pl[0].pixel = (uint32_t *)_pl_0;
	pl[1].W = 3; pl[1].H = 3; pl[1].pixel = (uint32_t *)_pl_1;
	pl[2].W = 2; pl[2].H = 2; pl[2].pixel = (uint32_t *)_pl_2;
	pl[3].W = 3; pl[3].H = 3; pl[3].pixel = (uint32_t *)_pl_3;
	pl[4].W = 3; pl[4].H = 3; pl[4].pixel = (uint32_t *)_pl_4;

}

uint32_t _Temp[MIN_DIST * 3][MIN_DIST * 3];
void piece_rotate(_Piece *piece)
{
	void block_copy(uint32_t *pixel, int x1, int y1, int x2, int y2, int width)
	{
		for (int j = 0; j < MIN_DIST; j++)
			for (int i = 0; i < MIN_DIST; i++)
				_Temp[j + y2][i + x2] = *(pixel + (y1 + j) * width + (x1 + i));
	};
	//Copy (x1, y1, x1 + MIN_DIST, y1 + MIN_DIST) in pixel to (x2, y2, x2 + MIN_DIST, y2 + MIN_DIST)
	int rotatable = 1;
	int x = piece->x;
	int y = piece->y;
	if (piece->w == 2 * MIN_DIST)
	{
		block_copy(piece->pixel, 0, 0, MIN_DIST, 0, 2 * MIN_DIST);
		block_copy(piece->pixel, MIN_DIST, 0, MIN_DIST, MIN_DIST, 2 * MIN_DIST);
		block_copy(piece->pixel, MIN_DIST, MIN_DIST, 0, MIN_DIST, 2 * MIN_DIST);
		block_copy(piece->pixel, 0, MIN_DIST, 0, 0, 2 * MIN_DIST);
		for (int j = 0; j < 2 * MIN_DIST; j++)
			for (int i = 0; i < 2 * MIN_DIST; i++)
			{
				if (_Temp[j][i] != 0 && y + j < SCREEN_HEIGHT && x + i < SCREEN_WIDTH && trs.bg[y + j][x + i] != 0)
					rotatable = 0;
			else if (_Temp[j][i] != 0 && !(y + j < SCREEN_HEIGHT && x + i < SCREEN_WIDTH))
				rotatable = 0;
			}
		if (rotatable)
			for (int j = 0; j < 2 * MIN_DIST; j++)
				for (int i = 0; i < 2 * MIN_DIST; i++)
					*(piece->pixel + j * 2 * MIN_DIST + i) = _Temp[j][i];
	}
	else if (piece->w == 3 * MIN_DIST)
	{
		block_copy(piece->pixel, 0, 0, MIN_DIST, 0, 3 * MIN_DIST);
		block_copy(piece->pixel, MIN_DIST, 0, 2 * MIN_DIST, 0, 3 * MIN_DIST);
		block_copy(piece->pixel, 2 * MIN_DIST, 0, 2 * MIN_DIST, MIN_DIST, 3 * MIN_DIST);
		block_copy(piece->pixel, 2 * MIN_DIST, MIN_DIST, 2 * MIN_DIST, 2 * MIN_DIST, 3 * MIN_DIST);
		block_copy(piece->pixel, 2 * MIN_DIST, 2 * MIN_DIST, MIN_DIST, 2 * MIN_DIST, 3 * MIN_DIST);
		block_copy(piece->pixel, MIN_DIST, 2 * MIN_DIST, 0, 2 * MIN_DIST, 3 * MIN_DIST);
		block_copy(piece->pixel, 0, 2 * MIN_DIST, 0, MIN_DIST, 3 * MIN_DIST);
		block_copy(piece->pixel, 0, MIN_DIST, 0, 0, 3 * MIN_DIST);
		
		for (int j = 0; j < 3 * MIN_DIST; j++)
			for (int i = 0; i < 3 * MIN_DIST; i++)
			{
				if (_Temp[j][i] != 0 && y + j < SCREEN_HEIGHT && x + i < SCREEN_WIDTH && trs.bg[y + j][x + i] != 0)
					rotatable = 0;
				else if (_Temp[j][i] != 0 && !(y + j < SCREEN_HEIGHT && x + i < SCREEN_WIDTH))
					rotatable = 0;
			}
		if (rotatable)
			for (int j  = 0; j < 3 * MIN_DIST; j++)
				for (int i = 0; i < 3 * MIN_DIST; i++)
					*(piece->pixel + j * 3 * MIN_DIST + i) = _Temp[j][i];
	}
}

