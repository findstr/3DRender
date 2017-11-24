#include <stdio.h>
#include "math.h"
#include "graphic.h"

#define WIDTH	(640)
#define HEIGHT	(480)
#define BITDEPTH (24)

static void
display(void)
{
	uint8_t *frame = graphic_lock_secondary();
	int i, j;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			uint8_t c = ((((i&0x8)==0)^(((j&0x8))==0)))*255;
			uint8_t *buf = frame + (i * WIDTH + j) * (BITDEPTH / 8);
			buf[0] = c;
			buf[1] = c;
			buf[2] = c;
		}
	}
	graphic_unlock_secondary(frame);
	graphic_flip_display();
}

int main(int argc, char **argv)
{
	graphic_run(WIDTH, HEIGHT, BITDEPTH, display);
	return 0;
}
