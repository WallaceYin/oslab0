#include <am.h>
#include <amdev.h>
#include <ylib.h>
#include <device.h>
#include <tetris.h>
#define FPS 20

#define TEST

#ifdef TEST
static void input_test(_Device *dev);
static void timer_test(_Device *dev);
static void video_test(_Device *dev);
static void pciconf_test(_Device *dev);
static void ata_test(_Device *dev);
#endif

void print_message(void);
int main() {
  if (_ioe_init() != 0) _halt(1);
#ifdef TEST
  printf("_heap = [%08x, %08x)\n", _heap.start, _heap.end);
	int *p = _heap.start;
	printf("duiqui: %d\n", *p);
  for (int n = 1; ; n++) {
    _Device *dev = _device(n);
    if (!dev) break;
    printf("*Device: %s\n", dev->name);
    switch (dev->id) {
      case _DEV_INPUT: input_test(dev); break;
      case _DEV_TIMER: timer_test(dev); break;
      case _DEV_VIDEO: video_test(dev); break;
      case _DEV_PCICONF: pciconf_test(dev); break;
      case _DEV_ATA0: ata_test(dev); break;
    }
    printf("\n");
  }
#else
	unsigned next_frame = 1000 / FPS;
	int key;
	print_message();
	game_block();
	time_base();
	for (;;)
	{
		/*while(up_time() < next_frame);
		while ((key = read_key()) != _KEY_NONE)
		{
			kbd_event(key);
		}*/
		while (up_time() < next_frame)
			if ((key = read_key()) != _KEY_NONE)
				kbd_event(key);
		game_process();
		screen_update();
		next_frame += 1000 / FPS;
	}

#endif
  return 0;
}

#ifdef TEST
static void input_test(_Device *dev) {
  printf("Input device test skipped.\n");
	/*int key;
	for (;;)
	{
		key = read_key();
		if (key != _KEY_NONE)
			printf("keycode received: %04x\n", key);
		if (key == _KEY_ESCAPE)
			break;
	}*/
}

static void timer_test(_Device *dev) {
  uint32_t t0;

	/*
  dev->read(_DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
  t0 = uptime.lo;
	*/
	time_base();
	t0 = up_time();
	return; 
  //for (int volatile i = 0; i < 10000000; i ++) ;

	/*
  dev->read(_DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
  t1 = uptime.lo;
	*/
	//t1 = up_time();

  //printf("Loop 10^7 time elapse: %d ms\n", t1 - t0);
	for (;;)
	{
		while (t0 < 1000)
			t0 = up_time();
		printf("1s passed\n");
		time_base();
		t0 = up_time();
	}
}

uint32_t pix[480][640];
static void video_test(_Device *dev) {
	/*
  _VideoInfoReg info;
  dev->read(_DEVREG_VIDEO_INFO, &info, sizeof(info));
	*/
	int width, height;
	width = screen_width();
	height = screen_height();
  printf("Screen size: %d x %d\n", width, height);
	//uint32_t pixel = 0x006a005f;
  /*for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++) 
			draw_rect(&pixel, width / 2 - 50 + x, height / 2 - 50 + y, 1, 1);
	//draw_sync();
	pixel = 0x006a0000;
	for (int x = 0; x < 100; x++)
		for (int y = 0; y < 100; y++)
			draw_rect(&pixel, x, y, 1, 1);
	//draw_sync();*/
	for (int i = 0; i < 640; i++)
		for (int j = 0; j < 480; j++)
			pix[j][i] = 0x006a005f;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
			pix[240 - 50 + j][320 - 50 + i] = 0x00006a4a;
	draw_rect((uint32_t *)pix, 0, 0, 640, 480);
  printf("You should see a purple square on the screen.\n");
}

static uint32_t pci_conf_read(_Device *dev, uint8_t bus, uint8_t slot,
                              uint8_t func, uint8_t offset) {
  uint32_t res;
  dev->read(_DEVREG_PCICONF(bus, slot, func, offset), &res, 4);
  return res;
}

static void pciconf_test(_Device *dev) {
  for (int bus = 0; bus < 256; bus ++)
    for (int slot = 0; slot < 32; slot ++) {
      uint32_t info = pci_conf_read(dev, bus, slot, 0, 0);
      uint16_t id = info >> 16, vendor = info & 0xffff;
      if (vendor != 0xffff) {
        printf("Get device %d:%d, id %x vendor %x", bus, slot, id, vendor);
        if (id == 0x100e && vendor == 0x8086) {
          printf(" <-- This is an Intel e1000 NIC card!");
        }
        printf("\n");
      }
    }
}

static uint8_t readb(_Device *dev, uint32_t reg) {
  uint8_t res;
  dev->read(reg, &res, 1);
  return res;
}

static uint32_t readl(_Device *dev, uint32_t reg) {
  uint32_t res;
  dev->read(reg, &res, 4);
  return res;
}

static void writeb(_Device *dev, uint32_t reg, uint8_t res) {
  dev->write(reg, &res, 1);
}

#define SECTSZ 512

static void ata_test(_Device *dev) {
  int offset = 0;
  while ((readb(dev, _DEVREG_ATA_STATUS) & 0xc0) != 0x40);
  writeb(dev, _DEVREG_ATA_NSECT,  1);
  writeb(dev, _DEVREG_ATA_SECT,   offset);
  writeb(dev, _DEVREG_ATA_CYLOW,  offset >> 8);
  writeb(dev, _DEVREG_ATA_CYHIGH, offset >> 16);
  writeb(dev, _DEVREG_ATA_DRIVE,  (offset >> 24) | 0xe0);
  writeb(dev, _DEVREG_ATA_STATUS, 0x20);
  while ((readb(dev, _DEVREG_ATA_STATUS) & 0xc0) != 0x40);
  uint32_t buf[SECTSZ / sizeof(uint32_t)];
  for (int i = 0; i < SECTSZ / sizeof(uint32_t); i++) {
    buf[i] = readl(dev, _DEVREG_ATA_DATA);
  }
  printf("Reading out the MBR:\n");
  for (int i = 0; i < SECTSZ / 16 / sizeof(uint16_t); i ++) {
    for (int j = 0; j < 16; j++) {
      printf("%04x ", ((uint16_t *)buf)[i * 16 + j] & 0xffff);
    }
    printf("\n");
  }
}
#endif

void print_message(void)
{
	printf("This is a Tetris game. Enjoy yourself!\n");
	printf("Description:\n");
	printf("<F2>: Game start\n");
	printf("<F3>: Pause/Continue\n");
	printf("<F5>: Game restart\n");
	printf("<A>: Left move\n");
	printf("<S>: Down move\n");
	printf("<D>: Right move\n");
	printf("<W>: Rotate\n");
	printf("WARNING: Please run this in 640 * 480 size.\n");
}

