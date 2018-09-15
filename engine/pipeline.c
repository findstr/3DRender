#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "mathlib.h"
#include "primitive.h"
#include "camera.h"
#include "pipeline.h"
#include "device.h"
#include "shader.h"
#include "light.h"


struct pipeline_buffer {
	vertex_t *buf;
	size_t size;
};

struct pipeline {
	void (*update)();
	struct object *render;
	struct camera *camera;
	struct pipeline_buffer TRANS;
};

static struct pipeline ENG;

static vertex_t *
getbuf(struct pipeline_buffer *buff, size_t sz)
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
	transform_t *trans = &obj->transform;
	for (i = 0; i < obj->vertices_num; i++) {
		vertex_t *vert = &src[i];
		vector4_mul_quaternion(&vert->v, &trans->rot, &vert->app.position);
		vector4_mul_quaternion(&vert->n, &trans->rot, &vert->app.normal);
		vector4_add(&vert->app.position, &trans->pos, &vert->app.position);
		vert->app.texcoord0 = vert->t;
		assert(vert->t.x <= 1.0f);
		assert(vert->t.y <= 1.0f);
	}
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		vector4_mul_quaternion(&p->normal_local, &trans->rot, &p->normal);
	}
	return ;
}

static void
transform_obj(struct object *obj, struct shader_global *G)
{
	int i;
	shader_vert_t *vert = obj->martial.shader.vert;
	vertex_t *list = obj->vlist_local;
	for (i = 0; i < obj->vertices_num; i++)
		vert(&list[i].app, &list[i].v2f, G);
	return ;
}

struct trapezoid {
	struct shader_v2f left[2];
	struct shader_v2f right[2];
};

static inline void
top_trapezoid(struct shader_v2f *ver0, struct shader_v2f *ver1,
	struct shader_v2f *ver2, struct trapezoid *r)
{
	if (ver0->sv_position.x > ver1->sv_position.x) {
		struct shader_v2f *tmp;
		SWAP(ver0, ver1, tmp);
	}
	r->left[0] = *ver0;
	r->left[1] = *ver2;
	r->right[0] = *ver1;
	r->right[1] = *ver2;
}

static inline void
bottom_trapezoid(struct shader_v2f *ver0, struct shader_v2f *ver1,
	struct shader_v2f *ver2, struct trapezoid *r)
{
	if (ver1->sv_position.x > ver2->sv_position.x) {
		struct shader_v2f *tmp;
		SWAP(ver1, ver2, tmp);
	}
	r->left[0] = *ver0;
	r->left[1] = *ver1;
	r->right[0] = *ver0;
	r->right[1] = *ver2;
}

static inline int
tri_to_trapezoid(struct tri *p, struct trapezoid r[2])
{
	struct shader_v2f *tmp;
	struct shader_v2f *ver0, *ver1, *ver2;
	ver0 = &p->vlist[p->vert[0]].v2f;
	ver1 = &p->vlist[p->vert[1]].v2f;
	ver2 = &p->vlist[p->vert[2]].v2f;
	if (FCMP(ver0->sv_position.x, ver1->sv_position.x) && FCMP(ver1->sv_position.x, ver2->sv_position.x))
		return 0;
	if ((FCMP(ver0->sv_position.y, ver1->sv_position.y) && FCMP(ver1->sv_position.y, ver2->sv_position.y)))
		return 0;
	//根据y坐标升序排p0, p1, p2
	if (ver1->sv_position.y < ver0->sv_position.y)
		SWAP(ver0, ver1, tmp);
	//此时p0 < p1
	if (ver2->sv_position.y < ver1->sv_position.y) {
		SWAP(ver2, ver1, tmp);
		if (ver1->sv_position.y < ver0->sv_position.y)
			SWAP(ver0, ver1, tmp);
	}
	assert(ver0->sv_position.y <= ver1->sv_position.y);
	assert(ver1->sv_position.y <= ver2->sv_position.y);
	if (FCMP(ver0->sv_position.y, ver1->sv_position.y)) {
		top_trapezoid(ver0, ver1, ver2, &r[0]);
		return 1;
	} else if (FCMP(ver1->sv_position.y, ver2->sv_position.y)) {
		bottom_trapezoid(ver0, ver1, ver2, &r[0]);
		return 1;
	} else {
		struct shader_v2f medium;
		float h = ver2->sv_position.y - ver0->sv_position.y;
		float newh = ver1->sv_position.y - ver0->sv_position.y;
		shader_v2f_lerp(ver0, ver2, newh / h, &medium);
		bottom_trapezoid(ver0, &medium, ver1, &r[0]);
		top_trapezoid(&medium, ver1, ver2, &r[1]);
		return 2;
	}
}

static inline void
render_trapezoid(struct trapezoid *r, shader_frag_t *frag, struct shader_global *G)
{
	float x, y, top, bottom;
	float ystart, yheight;
	assert(FCMP(r->left[0].sv_position.y, r->right[0].sv_position.y));
	assert(FCMP(r->left[1].sv_position.y, r->right[1].sv_position.y));
	ystart = r->left[0].sv_position.y;
	yheight =  r->left[1].sv_position.y - ystart;
	top = ceil(ystart);
	bottom = ceil(r->left[1].sv_position.y);
	for (y = top; y < bottom; y += 1.0f) {
		struct shader_v2f vleft, vright;
		float lerp, left, right;
		float x, xstart, xwidth;
		lerp = (y - ystart) / yheight;
		shader_v2f_lerp(&r->left[0], &r->left[1], lerp, &vleft);
		shader_v2f_lerp(&r->right[0], &r->right[1], lerp, &vright);
		xstart = vleft.sv_position.x; 
		xwidth = vright.sv_position.x - vleft.sv_position.x;
		left = ceil(vleft.sv_position.x);
		right = ceil(vright.sv_position.x);
		for (x = left; x < right; x += 1.0f) {
			rgba_t color;
			float lerp, z;
			struct shader_v2f i;
			lerp = (x - xstart) / xwidth;
			shader_v2f_lerp(&vleft, &vright,  lerp, &i);
			z = 1 / i.sv_position.z;
			i.texcoord0.x *= z;
			i.texcoord0.y *= z;
			color = frag(&i, G);
			device_drawpixel((int)x, (int)y, color, i.sv_position.z);
		}
	}
}

void
pipeline_frag_obj(struct object *obj, struct shader_global *G)
{
	int n;
	shader_frag_t *frag;
	struct trapezoid r[2];
	struct tri *p = obj->rlist;
	G->BITMAP = &obj->martial.texture;
	frag = obj->martial.shader.frag;
	while (p) {
		p->vlist = obj->vlist_local;
		n = tri_to_trapezoid(p, r);
		switch (n) {
		case 2:
			render_trapezoid(&r[1], frag, G);
			//fall through
		case 1:
			render_trapezoid(&r[0], frag, G);
			break;
		}
		p = p->next;
	}
}

static void
pipeline_vert()
{
	struct shader_global G;
	struct camera *c = ENG.camera;
	G.MVP = c->mcam;
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			obj->rlist = NULL;
			model2world(obj);
			camera_backface(c, obj);
			camera_transform(c);
			//light_transform(c, obj);
			transform_obj(obj, &G);
			obj = obj->next;
		}
		c = c->next;
	}
}

static void
pipeline_frag()
{
	struct shader_global G;
	struct camera *c = ENG.camera;
	G.MVP = c->mcam;
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			camera_perspective(c, obj);
			camera_viewport(c, obj);
			pipeline_frag_obj(obj, &G);
			obj = obj->next;
		}
		c = c->next;
	}
}

static void
pipeline_pipeline()
{
	if (ENG.update)
		ENG.update();
	device_clear();
	pipeline_vert();
	pipeline_frag();
	device_flip();
}

void
pipeline_add_camera(struct camera *cam)
{
	cam->next = ENG.camera;
	ENG.camera = cam;
	return ;
}

void
pipeline_add_object(struct object *obj)
{
	obj->next = ENG.render;
	ENG.render = obj;
	return ;
}

void
pipeline_run()
{
	driver_run();
	return ;
}

void
pipeline_start(int width, int height, void (*update)())
{
	ENG.render = NULL;
	ENG.camera = NULL;
	ENG.update = update;
	ENG.TRANS.size = 0;
	ENG.TRANS.buf = NULL;
	device_init(width, height);
	driver_start(width, height, pipeline_pipeline);
	return ;
}

