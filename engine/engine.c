#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "mathlib.h"
#include "primitive.h"
#include "camera.h"
#include "engine.h"
#include "device.h"
#include "light.h"


struct engine {
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

void
draw(struct object *obj)
{
	struct tri *p = obj->rlist;
	while (p) {
		p->vlist = obj->vlist_trans;
		device_drawframe(p);
		device_draw(p);
		p = p->next;
	}
}



static void
engine_render()
{
	struct camera *c = ENG.camera;
	if (ENG.update)
		ENG.update();
	device_clear();
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			vector4_t *trans = getbuf(obj->vertices_num);
			obj->vlist_trans = trans;
			obj->rlist = NULL;
			obj->next = NULL;
			model2world(obj);
			camera_backface(c, obj);
			light_transform(c, obj);
			camera_transform(c, obj);
			draw(obj);
			obj->vlist_trans = NULL;
			obj->next = NULL;
			obj = obj->next;
		}
		c = c->next;
	}
	device_flip();
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
	driver_run();
	return ;
}

void
engine_start(int width, int height, void (*update)())
{
	ENG.render = NULL;
	ENG.camera = NULL;
	ENG.update = update;
	BUFF.size = 0;
	BUFF.buf = NULL;
	device_init(width, height);
	driver_start(width, height, engine_render);
	return ;
}

