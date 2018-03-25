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
	//printf("draw pixel:%d %d\n", x, y);
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
	bitmap_load("resource/wall01.bmp", &BITMAP);
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

struct render {
	float ytop;
	float ybottom;
	float xleft;	//left
	float xright;
	float xlstep;
	float xrstep;
	vector2_t tleft; //texture
	vector2_t tright;
	vector2_t tlstep;
	vector2_t trstep;
};

static inline void
top_texture(vertex_t *ver0, vertex_t *ver1, vertex_t *ver2, struct render *r)
{
	float h, width, height;
	float u0, u1, u2, v0, v1, v2;
	if (ver0->v.x > ver1->v.x) {
		vertex_t *tmp;
		SWAP(ver0, ver1, tmp);
	}
	width = BITMAP.info.width - 1;
	height = BITMAP.info.height - 1;
	r->ytop = ver0->v.y;
	r->ybottom = ver2->v.y;
	r->xleft = ver0->v.x;
	r->xright = ver1->v.x;
	h = ver2->v.y - ver0->v.y;
	r->xlstep = (ver2->v.x - ver0->v.x) / h; //left
	r->xrstep = (ver2->v.x - ver1->v.x) / h; //right
	u0 = width * ver0->t.x; v0 = height * ver0->t.y;
	u1 = width * ver1->t.x; v1 = height * ver1->t.y;
	u2 = width * ver2->t.x; v2 = height * ver2->t.y;
	r->tleft.x = u0; r->tleft.y = v0;
	r->tright.x = u1; r->tright.y = v1;
	r->tlstep.x = (u2 - u0) / h;
	r->tlstep.y = (v2 - v0) / h;
	r->trstep.x = (u2 - u1) / h;
	r->trstep.y = (v2 - v1) / h;
	return ;
}

static inline void
bottom_texture(vertex_t *ver0, vertex_t *ver1, vertex_t *ver2, struct render *r)
{
	float h, width, height;
	float u0, u1, u2, v0, v1, v2;
	if (ver1->v.x > ver2->v.x) {
		vertex_t *tmp;
		SWAP(ver1, ver2, tmp);
	}
	width = BITMAP.info.width - 1;
	height = BITMAP.info.height - 1;
	r->ytop = ver0->v.y;
	r->ybottom = ver2->v.y;
	r->xleft = ver0->v.x;
	r->xright = ver0->v.x;
	h = ver2->v.y - ver0->v.y;
	r->xlstep = (ver1->v.x - ver0->v.x) / h; //left
	r->xrstep = (ver2->v.x - ver0->v.x) / h; //right
	u0 = width * ver0->t.x; v0 = height * ver0->t.y;
	u1 = width * ver1->t.x; v1 = height * ver1->t.y;
	u2 = width * ver2->t.x; v2 = height * ver2->t.y;
	r->tleft.x = u0; r->tleft.y = v0;
	r->tright.x = u0; r->tright.y = v0;
	r->tlstep.x = (u1 - u0) / h;
	r->tlstep.y = (v1 - v0) / h;
	r->trstep.x = (u2 - u0) / h;
	r->trstep.y = (v2 - v0) / h;
	return ;
}

static inline void
render_texture(struct render *r)
{
	int x, y;
	rgba_t *color = BITMAP.buffer;
	int heightn = BITMAP.info.height;
	int widthn = BITMAP.info.width;
	float xstart = r->xleft, xend = r->xright;
	float ustart = r->tleft.x, vstart = r->tleft.y;
	float uend = r->tright.x, vend = r->tright.y;
	float dl_x = r->xlstep, dr_x = r->xrstep;
	float dl_u = r->tlstep.x, dl_v = r->tlstep.y;
	float dr_u = r->trstep.x, dr_v = r->trstep.y;
	float delta_y = ceil(r->ytop) - r->ytop;
	xstart += delta_y * dl_x;
	xend += delta_y * dr_x;
	ustart += delta_y * dl_u; vstart += delta_y * dl_v;
	uend += delta_y * dr_u; vend += delta_y * dr_v;
	for (y = ceil(r->ytop); y < ceil(r->ybottom); y++) {
		float du, dv, u, v;
		float dx = xend - xstart;
		if (dx > 0) {
			du = (uend - ustart) / dx;
			dv = (vend - vstart) / dx;
		} else {
			du = uend - ustart;
			dv = vend - vstart;
		}
		u = ustart; v = vstart;
		for (x = ceil(xstart); x < ceil(xend); x++) {
			int uu, vv;
			uu = u; vv = v;
			assert(uu >= 0 && uu < widthn);
			assert(vv >= 0 && vv < heightn);
			draw_pixel(x, y, color[vv * widthn + uu]);
			u += du; v += dv;
		}
		xstart += dl_x;
		xend += dr_x;
		ustart += dl_u; uend += dr_u;
		vstart += dl_v; vend += dr_v;
	}
	return ;
}

void
device_draw(struct tri *p)
{
#if 1
	struct render r;
	rgba_t color[3], tc;
	vertex_t *ver0, *ver1, *ver2;
	ver0= &p->vlist[p->vert[0]];
	ver1 = &p->vlist[p->vert[1]];
	ver2 = &p->vlist[p->vert[2]];

	if (FCMP(ver0->v.x, ver1->v.x) && FCMP(ver1->v.x, ver2->v.x))
		return ;
	if ((FCMP(ver0->v.y, ver1->v.y) && FCMP(ver1->v.y, ver2->v.y)))
		return ;
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
		top_texture(ver0, ver1, ver2, &r);
		render_texture(&r);
	} else if (FCMP(ver1->v.y, ver2->v.y)) {
		bottom_texture(ver0, ver1, ver2, &r);
		render_texture(&r);
	} else {
		vertex_t medium;
		float h = ver2->v.y - ver0->v.y;
		float newh = ver1->v.y - ver0->v.y;
		float newx = ver0->v.x + newh * (ver2->v.x - ver0->v.x) / h;
		float newu = ver0->t.x + newh * (ver2->t.x - ver0->t.x) / h;
		float newv = ver0->t.y + newh * (ver2->t.y - ver0->t.y) / h;
		medium.v.x = newx;
		medium.v.y = ver1->v.y;
		medium.t.x = newu;
		medium.t.y = newv;
		bottom_texture(ver0, &medium, ver1, &r);
		render_texture(&r);
		top_texture(&medium, ver1, ver2, &r);
		render_texture(&r);
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

#endif

