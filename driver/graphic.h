#ifndef	_GRAPHIC_H
#define	_GRAPHIC_H

#include <stdint.h>

void graphic_run(int width, int height, void (*render)());
uint8_t *graphic_lock_primary();
uint8_t *graphic_lock_secondary();
void graphic_unlock_primary(uint8_t *buffer);
void graphic_unlock_secondary(uint8_t *buffer);
void graphic_fill_primary(int color);
void graphic_fill_secondary(int color);
void graphic_draw_pixel(int x, int y, int color);
void graphic_flip_display();

#endif

