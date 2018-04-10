#include <device.h>

uint32_t up_time(void)
{
	for (int n = 1;; n++)
	{
		_Device *dev = _device(n);
		if (!dev) break;
		if (dev->id == _DEV_TIMER)
		{
			_UptimeReg uptime;
			uint32_t t0;
			dev->read(_DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
			t0 = uptime.lo;
			return t0;
		}
		else
			continue;
	}
	return (uint32_t)-1;
}

int read_key(void)
{
	//TODO:
	return 1;
}

void draw_rect(uint32_t *pixels, int x, int y, int w, int h)
{
	//TODO: draw pixels
}

void draw_sync(void)
{
	//TODO
}

int screen_width(void)
{
	//TODO: get the screen width
	return 1;
}

int screen_height(void)
{
	//TODO: get the screen height
	return 1;
}
