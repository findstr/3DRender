#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"
#include "device.h"
#include "rgb.h"

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
draw_pixel(int x, int y, rgba_t color)
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
	*ptr++ = RGBA_R(color);
	*ptr++ = RGBA_G(color);
	*ptr++ = RGBA_B(color);
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
	vec0 = &p->vlist[v0].v;
	vec1 = &p->vlist[v1].v;
	vec2 = &p->vlist[v2].v;
	draw_line(vec0->x, vec0->y, vec1->x, vec1->y, 0x333333);
	draw_line(vec0->x, vec0->y, vec2->x, vec2->y, 0x333333);
	draw_line(vec1->x, vec1->y, vec2->x, vec2->y, 0x333333);
	return ;
}


#if 0

//edge function
void
device_draw(struct tri *p)
{
	int x, y;
	int v0, v1, v2;
	int lx, ty, rx, by;
	int x1, x2, x3, y1, y2, y3;
	int dx21, dx32, dx13, dy21, dy32, dy13;

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
	dx21 = x2 - x1; dx32 = x3 - x2; dx13 = x1 - x3;
	dy21 = y2 - y1; dy32 = y3 - y2; dy13 = y1 - y3;

	int X21 = dx21 * (by - y1) + dy21 * x1 - (lx - 1) * dy21; //- dy21 * x
	int X32 = dx32 * (by - y2) + dy32 * x2 - (lx - 1) * dy32; //- dy32 * x
	int X13 = dx13 * (by - y3) + dy13 * x3 - (lx - 1) * dy13; //- dy13 * x
	for (y = by - 1; y >= ty; y--) {
		X21 -= dx21;
		X32 -= dx32;
		X13 -= dx13;
		int x21 = X21;
		int x32 = X32;
		int x13 = X13;
		//Left-Top
		if (dy21 < 0 || (dy21 == 0 && dx21 > 0)) x21++;
		if (dy32 < 0 || (dy32 == 0 && dx32 > 0)) x32++;
		if (dy13 < 0 || (dy13 == 0 && dx13 > 0)) x13++;
		for (x = lx; x < rx; x++) {
			int r;
			x21 -= dy21;
			x32 -= dy32;
			x13 -= dy13;
			if (x21 < 0)
				continue;
			if (x32 < 0)
				continue;
			if (x13 < 0)
				continue;
			draw_pixel(x, y, p->color);
		}
	}

	return ;

}

#else

static inline void
draw_top(float x0, float y0, float x1, float y1, float x2, float y2, rgba_t color[3])
{
	rgba_t rgb, c0, c1, c2;
	float x, y, xstart, xend;
	float dl_x, dl_r, dl_g, dl_b, dr_x, dr_r, dr_g, dr_b;
	float left_r, left_g, left_b;
	float right_r, right_g, right_b;
	float bottom_r, bottom_g, bottom_b;
	float tmp, dx_left, dx_right;
	float h = y2 - y0;
	c0 = color[0]; c1 = color[1]; c2 = color[2];
	if (x0 > x1) {
		SWAP(x0, x1, tmp);
		SWAP(c0, c1, rgb);
	}
	dl_x = (x2-x0); dx_left = dl_x / h;
	dr_x = (x2-x1); dx_right = dr_x / h;
	//left color
	rgb = c0;
	left_r = RGBA_R(rgb); left_g = RGBA_G(rgb); left_b = RGBA_B(rgb);
	//right color
	rgb = c1;
	right_r = RGBA_R(rgb); right_g = RGBA_G(rgb); right_b = RGBA_B(rgb);
	//bottom color
	rgb = c2;
	bottom_r = RGBA_R(rgb); bottom_g = RGBA_G(rgb); bottom_b = RGBA_B(rgb);

	//delta color
	dl_r = -(left_r - bottom_r) / h;
	dl_g = -(left_g - bottom_g) / h;
	dl_b = -(left_b - bottom_b) / h;

	dr_r = -(right_r - bottom_r) / h;
	dr_g = -(right_g - bottom_g) / h;
	dr_b = -(right_b - bottom_b) / h;

	//draw
	xstart = x0; xend = x1;
	for (y = ceil(y0); y < ceil(y2); y++) {
		float r, g, b, rr, gg, bb, dx;
		dx = xend - xstart;
		if (dx > 0) {
			rr = (right_r - left_r) / dx;
			gg = (right_g - left_g) / dx;
			bb = (right_b - left_b) / dx;
		} else {
			rr = right_r - left_r;
			gg = right_g - left_g;
			bb = right_b - left_b;
		}
		r = left_r; g = left_g; b = left_b;
		for (x = xstart; x < xend; x++) {
			rgba_t c = RGBA((int)r, (int)g, (int)b, 255);
			draw_pixel(x, y, c);
			r += rr; g += gg; b += bb;
		}
		left_r += dl_r; left_g += dl_g; left_b += dl_b;
		right_r += dr_r; right_g += dr_g; right_b += dr_b;
		xstart += dx_left;
		xend += dx_right;
	}
	return ;
}

static inline void
draw_bottom(float x0, float y0, float x1, float y1, float x2, float y2, rgba_t color[3])
{
	rgba_t rgb, c0, c1, c2;
	float x, y, xstart, xend;
	float dl_x, dl_r, dl_g, dl_b, dr_x, dr_r, dr_g, dr_b;
	float left_r, left_g, left_b;
	float right_r, right_g, right_b;
	float top_r, top_g, top_b;
	float tmp, dx_left, dx_right;
	float h = y2 - y0;
	c0 = color[0]; c1 = color[1]; c2 = color[2];
	if (x1 > x2) {
		SWAP(x1, x2, tmp);
		SWAP(c1, c2, rgb);
	}
	dl_x = (x1 - x0); dx_left = dl_x / h;
	dr_x = (x2 - x0); dx_right = dr_x / h;
	//left color
	rgb = c1;
	left_r = RGBA_R(rgb); left_g = RGBA_G(rgb); left_b = RGBA_B(rgb);
	//top color
	rgb = c0;
	top_r = RGBA_R(rgb); top_g = RGBA_G(rgb); top_b = RGBA_B(rgb);
	//right color
	rgb = c2;
	right_r = RGBA_R(rgb); right_g = RGBA_G(rgb); right_b = RGBA_B(rgb);
	//delta color
	dl_r = (left_r - top_r) / h;
	dl_g = (left_g - top_g) / h;
	dl_b = (left_b - top_b) / h;

	dr_r = (right_r - top_r) / h;
	dr_g = (right_g - top_g) / h;
	dr_b = (right_b - top_b) / h;

	//draw
	xstart = x0; xend = x0;
	left_r = right_r = top_r;
	left_g = right_g = top_g;
	left_b = right_b = top_b;
	for (y = ceil(y0); y < ceil(y1); y++) {
		float r, g, b, rr, gg, bb, dx;
		dx = xend - xstart;
		if (dx > 0) {
			rr = (right_r - left_r) / dx;
			gg = (right_g - left_g) / dx;
			bb = (right_b - left_b) / dx;
		} else {
			rr = right_r - left_r;
			gg = right_g - left_g;
			bb = right_b - left_b;
		}
		r = left_r; g = left_g; b = left_b;
		for (x = xstart; x < xend; x++) {
			rgba_t c = RGBA((int)r, (int)g, (int)b, 255);
			draw_pixel(x, y, c);
			r += rr; g += gg; b += bb;
		}
		left_r += dl_r; left_g += dl_g; left_b += dl_b;
		right_r += dr_r; right_g += dr_g; right_b += dr_b;
		xstart += dx_left;
		xend += dx_right;
	}
	return ;
}

void
device_draw(struct tri *p)
{
#if 1
	int v0, v1, v2;
	rgba_t color[3], tc;
	float x0, x1, x2, y0, y1, y2, tmp;
	vector4_t *vec0, *vec1, *vec2;
	v0 = p->vert[0];
	v1 = p->vert[1];
	v2 = p->vert[2];
	vec0 = &p->vlist[v0].v;
	vec1 = &p->vlist[v1].v;
	vec2 = &p->vlist[v2].v;
	x0 = vec0->x; x1 = vec1->x; x2 = vec2->x;
	y0 = vec0->y; y1 = vec1->y; y2 = vec2->y;

	if (FCMP(x0, x1) && FCMP(x1, x2) || (FCMP(y0, y1) && FCMP(y1, y2)))
		return ;
	color[0] = RGBA(255, 0, 0, 255);
	color[1] = RGBA(0, 255, 0, 255);
	color[2] = RGBA(0, 0, 255, 255);

	//根据y坐标升序排p0, p1, p2
	if (y1 < y0) {
		SWAP(x0, x1, tmp);
		SWAP(y0, y1, tmp);
		SWAP(color[0], color[1], tc);
	}
	//此时p0 < p1
	if (y2 < y1) {
		SWAP(x2, x1, tmp);
		SWAP(y2, y1, tmp);
		SWAP(color[2], color[1], tc);
		if (y1 < y0) {
			SWAP(x0, x1, tmp);
			SWAP(y0, y1, tmp);
			SWAP(color[0], color[1], tc);
		}
	}
	assert(y0 <= y1);
	assert(y1 <= y2);
	if (FCMP(y0, y1)) {
		draw_top(x0, y0, x1, y1, x2, y2, color);
	} else if (FCMP(y1, y2)) {
		draw_bottom(x0, y0, x1, y1, x2, y2, color);
	} else {
		int newcolor[3];
		float r1, g1, b1, r2, g2, b2;
		float h = y2 - y0;
		float newh = y1 - y0;
		float newx = x0 + newh * (x2 - x0) / h;
		r1 = RGBA_R(color[0]);
		g1 = RGBA_G(color[0]);
		b1 = RGBA_B(color[0]);

		r2 = RGBA_R(color[2]);
		g2 = RGBA_G(color[2]);
		b2 = RGBA_B(color[2]);

		r1 += newh * (r2 - r1) / h;
		g1 += newh * (g2 - g1) / h;
		b1 += newh * (b2 - b1) / h;
		newcolor[0] = color[0];
		newcolor[1] = RGBA(r1, g1, b1, 255);
		newcolor[2] = color[1];
		draw_bottom(x0, y0, newx, y1, x1, y1, newcolor);
		newcolor[0] = RGBA(r1, g1, b1, 255);
		newcolor[1] = color[1];
		newcolor[2] = color[2];
		draw_top(newx, y1, x1, y1, x2, y2, newcolor);
	}
#else
#if 0
	{
	rgba_t c = RGBA(255, 0, 0, 255);
	int color[3];
	float x0, y0, x1, y1, x2, y2;
	color[0] = RGBA(255, 0, 0, 255);
	color[1] = RGBA(0, 255, 0, 255);
	color[2] = RGBA(0, 0, 255, 255);
	x0 = 300.f; y0 = 30.f; x1 = 0.f; y1 = 300.f; x2 = 600.f; y2 = 300.f;
	draw_line(x0, y0, x1, y1, c);
	draw_line(x2, y2, x1, y1, c);
	draw_line(x2, y2, x0, y0, c);
	draw_bottom(x0, y0, x1, y1, x2, y2, color);
	}
#endif
	{
	rgba_t c = RGBA(255, 0, 0, 255);
	int color[3];
	float x0, y0, x1, y1, x2, y2;
	color[0] = RGBA(255, 0, 0, 255);
	color[1] = RGBA(0, 255, 0, 255);
	color[2] = RGBA(0, 0, 255, 255);
	x0 = 0.f; y0 = 30.f; x1 = 600.f; y1 = 30.f; x2 = 300.f; y2 = 300.f;
	draw_line(x0, y0, x1, y1, c);
	draw_line(x2, y2, x1, y1, c);
	draw_line(x2, y2, x0, y0, c);
	draw_top(x0, y0, x1, y1, x2, y2, color);
	}


#endif
	return ;
}

#endif

