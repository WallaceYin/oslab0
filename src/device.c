#include <device.h>

/*
_device(1): 8279 Keyboard Controller
_device(2): RDTSC Timer / CMOS RTC
_device(3): Standard VGA Controller
_device(4): PCI Configuration
_device(5): ATA Disk Controller 0

*/

uint32_t up_time(void)
{
	_Device *dev = _device(2);
	_UptimeReg uptime;
	uint32_t t0;
	dev->read(_DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
	t0 = uptime.lo;
	return t0;
}

int read_key(void)
{
	_Device *dev = _device(1);
	_KbdReg readkey;
	int Keydown, Keycode;
	dev->read(_DEVREG_INPUT_KBD, &readkey, sizeof(readkey));
	Keydown = readkey.keydown;
	Keycode = readkey.keycode;
	if (!Keydown)
		return _KEY_NONE;
	return Keycode;
}

void draw_rect(uint32_t *pixels, int x, int y, int w, int h)
{
	_Device *dev = _device(3);
	_FBCtlReg fbctlreg;
	fbctlreg.x = x;
	fbctlreg.y = y;
	fbctlreg.w = w;
	fbctlreg.h = h;
	fbctlreg.pixels = pixels;
	fbctlreg.sync = 0;
	dev->write(_DEVREG, &fbctlreg, sizeof(fbctlreg));
}
//WARNING: Something may be wrong here.
void draw_sync(void)
{
	_Device *dev = _device(3);
	_FBCtlReg fbctlreg;
	fbctlreg.sync = 1;
	dev->write(_DEVREG, &fbctlreg, sizeof(fbctlreg));
}

int screen_width(void)
{
	_Device *dev = _device(3);
	_VideoInfoReg videoinforeg;
	int Width;
	dev->read(_DEVREG_VIDEO_INFO, &videoinforeg, sizeof(videoinforeg));
	Width = videoinforeg.width;
	return Width;
}

int screen_height(void)
{
	_Device *dev = _device(3);
	_VideoInfoReg videoinforeg;
	int Height;
	dev->read(_DEVREG_VIDEO_INFO, &videoinforeg, sizeof(videoinforeg));
	Height = videoinforeg.height;
	return Height;
}
