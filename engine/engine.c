#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "mathlib.h"
#include "primitive.h"
#include "camera.h"
#include "engine.h"
#include "device.h"
#include "light.h"


struct engine_buffer {
	vertex_t *buf;
	size_t size;
};

struct engine {
	void (*update)();
	struct object *render;
	struct camera *camera;
	struct engine_buffer TRANS;
};

static struct engine ENG;

static vertex_t *
getbuf(struct engine_buffer *buff, size_t sz)
{
	if (buff->size < sz) {
		buff->buf = realloc(buff->buf, sz * sizeof(*buff->buf));
		buff->size = sz;
	}
	return buff->buf;
}


static void
model2world(struct object *obj)
{
	int i;
	vertex_t *src = obj->vlist_local;
	vertex_t *dst = obj->vlist_trans;
	transform_t *trans = &obj->transform;
	for (i = 0; i < obj->vertices_num; i++) {
		vector4_mul_quaternion(&src[i].v, &trans->rot, &dst[i].v);
		vector4_mul_quaternion(&src[i].n, &trans->rot, &dst[i].n);
		vector4_add(&dst[i].v, &trans->pos, &dst[i].v);
		dst[i].t = src[i].t;
		assert(dst[i].t.x <= 1.0f);
		assert(dst[i].t.y <= 1.0f);
	}
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		vector4_mul_quaternion(&p->normal_local, &trans->rot, &p->normal);
	}
	return ;
}

void
draw(struct object *obj)
{
	struct tri *p = obj->rlist;
	while (p) {
		p->vlist = obj->vlist_trans;
		//device_drawframe(p);
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
			vertex_t *trans;
			trans = getbuf(&ENG.TRANS, obj->vertices_num);
			obj->vlist_trans = trans;
			obj->rlist = NULL;
			model2world(obj);
			camera_backface(c, obj);
			light_transform(c, obj);
			camera_transform(c, obj);
			draw(obj);
			obj->vlist_trans = NULL;
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
	ENG.TRANS.size = 0;
	ENG.TRANS.buf = NULL;
	device_init(width, height);
	driver_start(width, height, engine_render);
	return ;
}

