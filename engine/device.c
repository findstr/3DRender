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
	uint8_t *frame;
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


static void
draw_pixel(unsigned int x, unsigned int y, rgba_t color)
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
	y = DEV.height - y - 1;
	ptr = DEV.frame + x * RGB_SIZE + y * DEV.width * RGB_SIZE;
	*ptr++ = RGBA_R(color);
	*ptr++ = RGBA_G(color);
	*ptr++ = RGBA_B(color);
	return ;
}



void
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

static struct bitmap BITMAP;

void
device_init(size_t width, size_t height)
{
	DEV.width = width;
	DEV.height = height;
	DEV.frame = malloc(width * height * RGB_SIZE);
	bitmap_load("resource/std.bmp", &BITMAP);
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

struct trapezoid {
	vertex_t left[2];
	vertex_t right[2];
};

static inline void
top_trapezoid(vertex_t *ver0, vertex_t *ver1, vertex_t *ver2, struct trapezoid *r)
{
	if (ver0->v.x > ver1->v.x) {
		vertex_t *tmp;
		SWAP(ver0, ver1, tmp);
	}
	r->left[0] = *ver0;
	r->left[1] = *ver2;
	r->right[0] = *ver1;
	r->right[1] = *ver2;
}

static inline void
bottom_trapezoid(vertex_t *ver0, vertex_t *ver1, vertex_t *ver2, struct trapezoid *r)
{
	if (ver1->v.x > ver2->v.x) {
		vertex_t *tmp;
		SWAP(ver1, ver2, tmp);
	}
	r->left[0] = *ver0;
	r->left[1] = *ver1;
	r->right[0] = *ver0;
	r->right[1] = *ver2;
}

static rgba_t
texture_sample(float u, float v)
{
	int x = u * (BITMAP.info.width - 1);
	int y = v * (BITMAP.info.height - 1);
	return BITMAP.buffer[y * BITMAP.info.width + x];
}

static inline void
render_trapezoid(struct trapezoid *r)
{
	float x, y, top, bottom;
	float ystart, yheight;
	assert(FCMP(r->left[0].v.y, r->right[0].v.y));
	assert(FCMP(r->left[1].v.y, r->right[1].v.y));
	ystart = r->left[0].v.y;
	yheight =  r->left[1].v.y - ystart;
	top = ceil(ystart);
	bottom = ceil(r->left[1].v.y);
	for (y = top; y < bottom; y += 1.0f) {
		vertex_t vleft, vright;
		float lerp, left, right;
		float x, xstart, xwidth;
		lerp = (y - ystart) / yheight;
		vertex_lerp(&r->left[0], &r->left[1], lerp, &vleft);
		vertex_lerp(&r->right[0], &r->right[1], lerp, &vright);
		xstart = vleft.v.x; 
		xwidth = vright.v.x - vleft.v.x;
		left = ceil(vleft.v.x);
		right = ceil(vright.v.x);
		for (x = left; x < right; x += 1.0f) {
			float lerp;
			vector2_t uv;
			rgba_t color;
			lerp = (x - xstart) / xwidth;
			vector2_lerp(&vleft.t, &vright.t, lerp, &uv);
			color = texture_sample(uv.x, uv.y);
			draw_pixel((int)x, (int)y, color);
		}
	}
}

static inline int
tri_to_trapezoid(struct tri *p, struct trapezoid r[2])
{
	rgba_t color[3], tc;
	vertex_t *ver0, *ver1, *ver2;
	ver0= &p->vlist[p->vert[0]];
	ver1 = &p->vlist[p->vert[1]];
	ver2 = &p->vlist[p->vert[2]];
	if (FCMP(ver0->v.x, ver1->v.x) && FCMP(ver1->v.x, ver2->v.x))
		return 0;
	if ((FCMP(ver0->v.y, ver1->v.y) && FCMP(ver1->v.y, ver2->v.y)))
		return 0;
	//根据y坐标升序排p0, p1, p2
	if (ver1->v.y < ver0->v.y) {
		vertex_t *tmp;
		SWAP(ver0, ver1, tmp);
	}
	//此时p0 < p1
	if (ver2->v.y < ver1->v.y) {
		vertex_t *tmp;
		SWAP(ver2, ver1, tmp);
		if (ver1->v.y < ver0->v.y)
			SWAP(ver0, ver1, tmp);
	}
	assert(ver0->v.y <= ver1->v.y);
	assert(ver1->v.y <= ver2->v.y);
	if (FCMP(ver0->v.y, ver1->v.y)) {
		top_trapezoid(ver0, ver1, ver2, &r[0]);
		return 1;
	} else if (FCMP(ver1->v.y, ver2->v.y)) {
		bottom_trapezoid(ver0, ver1, ver2, &r[0]);
		return 2;
	} else {
		vertex_t medium;
		float h = ver2->v.y - ver0->v.y;
		float newh = ver1->v.y - ver0->v.y;
		vertex_lerp(ver0, ver2, newh / h, &medium);
		bottom_trapezoid(ver0, &medium, ver1, &r[0]);
		top_trapezoid(&medium, ver1, ver2, &r[1]);
		return 2;
	}
}



void
device_draw(struct tri *p)
{
#if 1
	int n;
	struct trapezoid r[2];
	n = tri_to_trapezoid(p, r);
	switch (n) {
	case 2:
		render_trapezoid(&r[1]);
		//fall through
	case 1:
		render_trapezoid(&r[0]);
		break;
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
#if 0
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
#if 0
	int x, y;
	rgba_t *ptr = BITMAP.buffer;
	for (y = 0; y < BITMAP.info.height; y++) {
		for (x = 0; x < BITMAP.info.width; x++)
			draw_pixel(x, y, *ptr++);
	}
#endif
#if 0
	{
	int color[3];
	vertex_t v[3];
	struct render r;
	rgba_t c = RGBA(255, 0, 0, 255);
	float x0, y0, x1, y1, x2, y2;
	color[0] = RGBA(255, 0, 0, 255);
	color[1] = RGBA(0, 255, 0, 255);
	color[2] = RGBA(0, 0, 255, 255);
	vector3_init(&v[0].v, 300.f, 0.f, 0.f);
	vector3_init(&v[1].v, 0.f, 300.f, 0.f);
	vector3_init(&v[2].v, 600.f, 300.f, 0.f);
	vector2_init(&v[0].t, 0.5f, 0.f);
	vector2_init(&v[1].t, 0.f, 0.5f);
	vector2_init(&v[2].t, 1.f, 0.5f);

	draw_line(v[0].v.x, v[0].v.y, v[1].v.x, v[1].v.y, c);
	draw_line(v[2].v.x, v[2].v.y, v[1].v.x, v[1].v.y, c);
	draw_line(v[2].v.x, v[2].v.y, v[0].v.x, v[0].v.y, c);
	bottom_texture(&v[0], &v[1], &v[2], &r);
	render_texture(&r);
	}
#endif
#if 1
	{
	int color[3];
	vertex_t v[3];
	struct render r;
	rgba_t c = RGBA(255, 0, 0, 255);
	float x0, y0, x1, y1, x2, y2;
	color[0] = RGBA(255, 0, 0, 255);
	color[1] = RGBA(0, 255, 0, 255);
	color[2] = RGBA(0, 0, 255, 255);
	vector3_init(&v[0].v, 0.f, 0.f, 0.f);
	vector3_init(&v[1].v, 600.f, 0.f, 0.f);
	vector3_init(&v[2].v, 300.f, 300.f, 0.f);
	vector2_init(&v[0].t, 0.f, 0.f);
	vector2_init(&v[1].t, 1.f, 0.f);
	vector2_init(&v[2].t, 0.5f, 0.5f);

	draw_line(v[0].v.x, v[0].v.y, v[1].v.x, v[1].v.y, c);
	draw_line(v[2].v.x, v[2].v.y, v[1].v.x, v[1].v.y, c);
	draw_line(v[2].v.x, v[2].v.y, v[0].v.x, v[0].v.y, c);
	top_texture(&v[0], &v[1], &v[2], &r);
	render_texture(&r);
	}
#endif


#endif
	return ;
}

