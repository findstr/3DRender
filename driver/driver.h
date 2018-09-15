#ifndef	_DRIVER_H
#define	_DRIVER_H

#include <stdlib.h>
#include <stdint.h>

#define DRIVER_KEY_NORMAL	(1 << 0)
#define DRIVER_KEY_CTRL		(1 << 1)
#define DRIVER_KEY_SHIFT	(1 << 2)
#define DRIVER_KEY_ALT		(1 << 3)

void driver_start(int width, int height, void (*render)(void));
void driver_run();
void driver_draw(const uint8_t *frame, size_t width, size_t height);
int driver_keydown(char k, int ctrl);

#endif

