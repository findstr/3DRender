#include <stdlib.h>
#include <string.h>
#include "driver.h"
#include "device.h"

struct device {
	size_t width;
	size_t height;
	uint8_t *frame;
};

static struct device DEV;

static int
min(int a, int b, int c)
{
	int r = a;
	if (r > b)
		r = b;
	if (r > c)
		r = c;
	return r;
}

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


static void
draw_pixel(int x, int y, int color)
{
	uint8_t *ptr;
	if (x >= DEV.width) {
		//printf("X over flow\n");
		return ;
	}
	if (y >= DEV.height) {
		//printf("Y over flow\n");
		return ;
	}
	//OpenGL use the left-bottom as (0,0)
	y = DEV.height - y;
	ptr = DEV.frame + x * RGB_SIZE + y * DEV.width * RGB_SIZE;
	*ptr++ = (color >> 16) & 0xff;
	*ptr++ = (color >> 8) & 0xff;
	*ptr++ = (color >> 0) & 0xff;
	return ;
}



static void
draw_line(int x1, int y1, int x2, int y2, int c)
{
	int x, y, rem = 0;
	if (x1 == x2 && y1 == y2) { //point
		draw_pixel(x1, y1, c);
	} else if (x1 == x2) {	//H line
		int inc = y2 > y1 ? 1 : -1;
		for (y = y1; y != y2; y += inc)
			draw_pixel(x1, y, c);
		draw_pixel(x1, y2, c);
	} else if (y1 == y2) { //Y line
		int inc = x2 > x1 ? 1 : -1;
		for (x = x1; x != x2; x += inc)
			draw_pixel(x, y1, c);
		draw_pixel(x2, y1, c);
	} else {
		int dx = abs(x1 - x2);
		int dy = abs(y1 - y2);
		if (dx >= dy) {
			if (x2 < x1) {
				SWAP(x1, x2, x);
				SWAP(y1, y2, y);
			}
			for (x = x1, y = y1; x <= x2; x++) {
				draw_pixel(x, y, c);
				rem += dy;
				if (rem >= dx) {
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					draw_pixel(x, y, c);
				}
			}
			draw_pixel(x2, y2, c);
		} else {
			if (y2 < y1) {
				SWAP(x1, x2, x);
				SWAP(y1, y2, y);
			}
			for (x = x1, y = y1; y <= y2; y++) {
				draw_pixel(x, y, c);
				rem += dx;
				if (rem >= dy) {
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					draw_pixel(x, y, c);
				}
			}
			draw_pixel(x2, y2, c);
		}
	}
}

void
device_init(size_t width, size_t height)
{
	DEV.width = width;
	DEV.height = height;
	DEV.frame = malloc(width * height * RGB_SIZE);
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
	memset(DEV.frame, 0x00, DEV.width * DEV.height * RGB_SIZE);
}

void
device_flip()
{
	driver_draw(DEV.frame, DEV.width, DEV.height);
}

void
device_drawframe(struct tri *p)
{
	int v0, v1, v2;
	vector4_t *vec0, *vec1, *vec2;
	v0 = p->vert[0];
	v1 = p->vert[1];
	v2 = p->vert[2];
	vec0 = &p->vlist[v0];
	vec1 = &p->vlist[v1];
	vec2 = &p->vlist[v2];
	draw_line(vec0->x, vec0->y, vec1->x, vec1->y, 0x333333);
	draw_line(vec0->x, vec0->y, vec2->x, vec2->y, 0x333333);
	draw_line(vec1->x, vec1->y, vec2->x, vec2->y, 0x333333);
	return ;
}

void
device_draw(struct tri *p)
{
	int x, y;
	int v0, v1, v2;
	int lx, ty, rx, by;
	int x1, x2, x3, y1, y2, y3;
	vector4_t *vec0, *vec1, *vec2;
	v0 = p->vert[0];
	v1 = p->vert[1];
	v2 = p->vert[2];
	vec0 = &p->vlist[v0];
	vec1 = &p->vlist[v1];
	vec2 = &p->vlist[v2];
	x1 = vec0->x; x2 = vec1->x; x3 = vec2->x;
	y1 = vec0->y; y2 = vec1->y; y3 = vec2->y;
	lx = min(x1, x2, x3);
	rx = max(x1, x2, x3);
	ty = min(y1, y2, y3);
	by = max(y1, y2, y3);
	for (y = ty; y < by; y++) {
		for (x = lx; x < rx; x++) {
			int r;
			r = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
			if (r < 0)
				continue;
			r = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2);
			if (r < 0)
				continue;
			r = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3);
			if (r < 0)
				continue;
			draw_pixel(x, y, p->color);
		}
	}

	return ;

}





