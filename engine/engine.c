#include <stdio.h>
#include "graphic.h"
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
	struct object *render;
	struct camera *camera;
};

struct engine ENG;

static void
model2world(struct object *obj)
{
	int i;
	vector4_t *src = obj->vlist_local;
	vector4_t *dst = obj->vlist_trans;
	for (i = 0; i < obj->vertices_num; i++)
		vector4_add(&src[i], &obj->world_pos, &dst[i]);
	return ;
}

static void
draw_pixel(int x, int y, int color)
{
	uint8_t *ptr;
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
	int i;
	for (i = 0; i < obj->polys_num; i++) {
		int v0, v1, v2;
		vector4_t *vec0, *vec1, *vec2;
		int state = obj->plist[i].state;
		if (!(state & POLY4D_STATE_ACTIVE) ||
			(state & POLY4D_STATE_CLIPPED) ||
			(state & POLY4D_STATE_BACKFACE))
			continue;
		v0 = obj->plist[i].vert[0];
		v1 = obj->plist[i].vert[1];
		v2 = obj->plist[i].vert[2];
		vec0 = &obj->vlist_trans[v0];
		vec1 = &obj->vlist_trans[v1];
		vec2 = &obj->vlist_trans[v2];
#if 0
		vector4_print("vec0", vec0);
		vector4_print("vec1", vec1);
		vector4_print("vec2", vec2);
#endif
		draw_line(vec0->x, vec0->y, vec1->x, vec1->y, obj->plist[i].color);
		draw_line(vec0->x, vec0->y, vec2->x, vec2->y, obj->plist[i].color);
		draw_line(vec1->x, vec1->y, vec2->x, vec2->y, obj->plist[i].color);
	}
}



static void
engine_render()
{
	struct object *obj = ENG.render;
	while (obj) {
		struct camera *c = ENG.camera;
		c = ENG.camera;
		model2world(obj);
		camera_transform(c, obj);
		draw(obj);
		obj = obj->next;
	}
	graphic_draw(ENG.frame, ENG.width, ENG.height);
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
	graphic_run(ENG.width, ENG.height, engine_render);
	return ;
}

void
engine_start(int width, int height)
{
	ENG.width = width;
	ENG.height = height;
	ENG.frame = malloc(width * height * RGB_SIZE);
	ENG.render = NULL;
	ENG.camera = NULL;
	return ;
}

