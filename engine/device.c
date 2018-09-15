#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rgb.h"
#include "mathlib.h"
#include "driver.h"
#include "bitmap.h"
#include "device.h"

struct device {
	int width;
	int height;
	rgba_t *frame;
	float *zbuffer;

};

static struct device DEV;

#ifndef min

static inline int
min(int a, int b, int c)
{
	int r = a;
	if (r > b)
		r = b;
	if (r > c)
		r = c;
	return r;
}
#endif


#ifndef max

static int
max(int a, int b, int c)
{
	int r = a;
	if (r < b)
		r = b;
	if (r < c)
		r = c;
	return r;
}

#endif


void
device_drawpixel(unsigned int x, unsigned int y, rgba_t color, float z)
{
	float *zp;
	if (x >= DEV.width) {
		//printf("X over flow\n");
		return ;
	}
	if (y >= DEV.height) {
		//printf("Y over flow\n");
		return ;
	}
	//OpenGL use the left-bottom as (0,0)
	y = DEV.height - y - 1;
	zp = DEV.zbuffer + x + y * DEV.width;
	if (*zp < z) {
		rgba_t *ptr;
		ptr = DEV.frame + x + y * DEV.width;
		*ptr = rgba_blend(color, *ptr);
		*zp = z;
	}
	return ;
}



void
draw_line(int x1, int y1, int x2, int y2, int c)
{
	int x, y, rem = 0;
	if (x1 == x2 && y1 == y2) { //point
		device_drawpixel(x1, y1, c, 1.0f);
	} else if (x1 == x2) {	//H line
		int inc = y2 > y1 ? 1 : -1;
		for (y = y1; y != y2; y += inc)
			device_drawpixel(x1, y, c, 1.0f);
		device_drawpixel(x1, y2, c, 1.0f);
	} else if (y1 == y2) { //Y line
		int inc = x2 > x1 ? 1 : -1;
		for (x = x1; x != x2; x += inc)
			device_drawpixel(x, y1, c, 1.0f);
		device_drawpixel(x2, y1, c, 1.0f);
	} else {
		int dx = abs(x1 - x2);
		int dy = abs(y1 - y2);
		if (dx >= dy) {
			if (x2 < x1) {
				SWAP(x1, x2, x);
				SWAP(y1, y2, y);
			}
			for (x = x1, y = y1; x <= x2; x++) {
				device_drawpixel(x, y, c, 1.0f);
				rem += dy;
				if (rem >= dx) {
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					device_drawpixel(x, y, c, 1.0f);
				}
			}
			device_drawpixel(x2, y2, c, 1.0f);
		} else {
			if (y2 < y1) {
				SWAP(x1, x2, x);
				SWAP(y1, y2, y);
			}
			for (x = x1, y = y1; y <= y2; y++) {
				device_drawpixel(x, y, c, 1.0f);
				rem += dx;
				if (rem >= dy) {
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					device_drawpixel(x, y, c, 1.0f);
				}
			}
			device_drawpixel(x2, y2, c, 1.0f);
		}
	}
}


void
device_init(size_t width, size_t height)
{
	DEV.width = width;
	DEV.height = height;
	DEV.frame = malloc(width * height * sizeof(rgba_t));
	DEV.zbuffer = malloc(width * height * sizeof(float)); 
	return ;
}

void
device_exit()
{
	free(DEV.frame);
	return ;
}

void
device_clear()
{
	size_t sz;
	float *fs, *fe;
	sz = DEV.width * DEV.height;
	fs = DEV.zbuffer;
	fe = fs + sz;
	memset(DEV.frame, 0x00, sz * sizeof(rgba_t));
	while (fs < fe)
		*fs++ = 0.0f;
	return ;
}

void
device_flip()
{
	driver_draw((uint8_t *)DEV.frame, DEV.width, DEV.height);
}

void
device_drawframe(struct tri *p)
{
	int v0, v1, v2;
	vector4_t *vec0, *vec1, *vec2;
	v0 = p->vert[0];
	v1 = p->vert[1];
	v2 = p->vert[2];
	vec0 = &p->vlist[v0].v2f.sv_position;
	vec1 = &p->vlist[v1].v2f.sv_position;
	vec2 = &p->vlist[v2].v2f.sv_position;
	draw_line(vec0->x, vec0->y, vec1->x, vec1->y, 0x333333);
	draw_line(vec0->x, vec0->y, vec2->x, vec2->y, 0x333333);
	draw_line(vec1->x, vec1->y, vec2->x, vec2->y, 0x333333);
	return ;
}

