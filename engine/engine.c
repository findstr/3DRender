#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "mathlib.h"
#include "primitive.h"
#include "camera.h"
#include "engine.h"

#define BIT_DEPTH	(24)
#define RGB_SIZE	(3)

struct engine {
	size_t width;
	size_t height;
	uint8_t *frame;
	void (*update)();
	struct object *render;
	struct camera *camera;
};

struct engine_buffer {
	vector4_t *buf;
	size_t size;
};

struct engine ENG;
struct engine_buffer BUFF;

static vector4_t *
getbuf(size_t sz)
{
	if (BUFF.size < sz) {
		BUFF.buf = realloc(BUFF.buf, sz * sizeof(*BUFF.buf));
		BUFF.size = sz;
	}
	return BUFF.buf;
}


static void
model2world(struct object *obj)
{
	int i;
	vector4_t *src = obj->vlist_local;
	vector4_t *dst = obj->vlist_trans;
	transform_t *trans = &obj->transform;
	for (i = 0; i < obj->vertices_num; i++) {
		vector4_mul_quaternion(&src[i], &trans->rot, &dst[i]);
		vector4_add(&dst[i], &trans->pos, &dst[i]);
	}
	return ;
}

static void
draw_pixel(int x, int y, int color)
{
	uint8_t *ptr;
	if (x >= ENG.width) {
		//printf("X over flow\n");
		return ;
	}
	if (y >= ENG.height) {
		//printf("Y over flow\n");
		return ;
	}
	//OpenGL use the left-bottom as (0,0)
	y = ENG.height - y;
	ptr = ENG.frame + x * RGB_SIZE + y * ENG.width * RGB_SIZE;
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
draw(struct object *obj)
{
	struct tri *p = obj->rlist;
	while (p) {
		int v0, v1, v2;
		vector4_t *vec0, *vec1, *vec2;
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];
		vec0 = &obj->vlist_trans[v0];
		vec1 = &obj->vlist_trans[v1];
		vec2 = &obj->vlist_trans[v2];
#if 0
		vector4_print("vec0", vec0);
		vector4_print("vec1", vec1);
		vector4_print("vec2", vec2);
#endif
		draw_line(vec0->x, vec0->y, vec1->x, vec1->y, p->color);
		draw_line(vec0->x, vec0->y, vec2->x, vec2->y, p->color);
		draw_line(vec1->x, vec1->y, vec2->x, vec2->y, p->color);
		p = p->next;
	}
}



static void
engine_render()
{
	struct camera *c = ENG.camera;
	if (ENG.update)
		ENG.update();
	memset(ENG.frame, 0x00, ENG.width * ENG.height * RGB_SIZE);
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			vector4_t *trans = getbuf(obj->vertices_num);
			obj->vlist_trans = trans;
			obj->rlist = NULL;
			obj->next = NULL;
			model2world(obj);
			camera_transform(c, obj);
			draw(obj);
			obj->vlist_trans = NULL;
			obj->next = NULL;
			obj = obj->next;
		}
		c = c->next;
	}
	driver_draw(ENG.frame, ENG.width, ENG.height);
}

void
engine_add_camera(struct camera *cam)
{
	cam->next = ENG.camera;
	ENG.camera = cam;
	return ;
}

void
engine_add_object(struct object *obj)
{
	obj->next = ENG.render;
	ENG.render = obj;
	return ;
}

void
engine_run()
{
	driver_run(ENG.width, ENG.height, engine_render);
	return ;
}

void
engine_start(int width, int height, void (*update)())
{
	ENG.width = width;
	ENG.height = height;
	ENG.frame = malloc(width * height * RGB_SIZE);
	ENG.render = NULL;
	ENG.camera = NULL;
	ENG.update = update;
	BUFF.size = 0;
	BUFF.buf = NULL;
	return ;
}

