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
	float time;
	vector2_t screen;
	struct object *render;
	struct camera *camera;
	struct pipeline_buffer TRANS;
	void (*update)();
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
		assert(medium.texcoord0.x * medium.texcoord0.y >= 0.0f);
		bottom_trapezoid(ver0, &medium, ver1, &r[0]);
		top_trapezoid(&medium, ver1, ver2, &r[1]);
		return 2;
	}
}

static inline void
render_trapezoid(struct trapezoid *r, shader_frag_t *frag, struct shader_global *G)
{
	float y, top, bottom;
	float ystart, yheight;
	assert(FCMP(r->left[0].sv_position.y, r->right[0].sv_position.y));
	assert(FCMP(r->left[1].sv_position.y, r->right[1].sv_position.y));
	ystart = r->left[0].sv_position.y;
	yheight =  r->left[1].sv_position.y - ystart;
	top = ceilf(ystart);
	bottom = ceilf(r->left[1].sv_position.y);
	for (y = top; y < bottom; y += 1.0f) {
		struct shader_v2f vleft, vright;
		float lerp, left, right;
		float x, xstart, xwidth;
		lerp = (y - ystart) / yheight;
		shader_v2f_lerp(&r->left[0], &r->left[1], lerp, &vleft);
		shader_v2f_lerp(&r->right[0], &r->right[1], lerp, &vright);
		xstart = vleft.sv_position.x;
		xwidth = vright.sv_position.x - vleft.sv_position.x;
		left = ceilf(vleft.sv_position.x);
		right = ceilf(vright.sv_position.x);
		for (x = left; x < right; x += 1.0f) {
			rgba_t color;
			float lerp, rhw;
			struct shader_v2f i;
			lerp = (x - xstart) / xwidth;
			shader_v2f_lerp(&vleft, &vright,  lerp, &i);
			rhw = i.sv_position.w;
			i.texcoord0.x /= rhw;
			i.texcoord0.y /= rhw;
			color = frag(&i, G);
			device_drawpixel((int)x, (int)y, color, rhw);
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
		p->vlist = obj->vlist;
		//device_drawframe(p);
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
transform_obj(struct object *obj, struct shader_global *G)
{
	int i;
	vertex_t *list = obj->vlist;
	transform_t *trans = &obj->transform;
	shader_vert_t *vertfunc = obj->martial.shader.vert;
	for (i = 0; i < obj->vertices_num; i++) {
		vertex_t *vert = &list[i];
		vector3_t *v = &vert->v;
		vector4_init(&vert->app.position, v->x, v->y, v->z);
		vert->app.texcoord0 = vert->t;
		vertfunc(&vert->app, &vert->v2f, G);
		vert->v2f.sv_position.w = 1.f / vert->v2f.sv_position.w; //RHW
		vert->v2f.texcoord0.x *= vert->v2f.sv_position.w;
		vert->v2f.texcoord0.y *= vert->v2f.sv_position.w;
	}
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		vector4_mul_matrix(&p->normal_local, &G->MVP, &p->normal);
	}
	return ;
}



static void
pipeline_vert()
{
	struct shader_global G;
	struct camera *c = ENG.camera;
	G.TIME = ENG.time;
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			matrix_t tmp;
			transform_t *trans = &obj->transform;
			vector3_t *scale = &obj->transform.scale;
			matrix_t mtrans = (matrix_t){
				scale->x, 0, 0, 0,
				0, scale->y, 0, 0,
				0, 0, scale->z, 0,
				trans->pos.x, trans->pos.y, trans->pos.z, 1.0f
			};
			obj->rlist = NULL;
			camera_transform(c);
			quaternion_to_matrix(&obj->transform.rot, &tmp);
			matrix_mul(&tmp, &mtrans, &tmp);
			matrix_mul(&tmp, &c->mcam, &G.MVP);
			transform_obj(obj, &G);
			camera_backface(c, obj);
			camera_viewport(c, obj);
			pipeline_frag_obj(obj, &G);
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
	G.SCREEN = ENG.screen;
	G.TIME = ENG.time;
#if 0
	while (c) {
		struct object *obj = ENG.render;
		while (obj) {
			camera_viewport(c, obj);
			pipeline_frag_obj(obj, &G);
			obj = obj->next;
		}
		c = c->next;
	}
#endif
}

static void
pipeline_update(void)
{
	if (ENG.update)
		ENG.update();
	device_clear();
	pipeline_vert();
	pipeline_frag();
	device_flip();
	ENG.time += 0.01f;
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
	ENG.time = 0;
	ENG.TRANS.size = 0;
	ENG.TRANS.buf = NULL;
	ENG.screen.x = (float)width;
	ENG.screen.y = (float)height;
	device_init(width, height);
	driver_start(width, height, pipeline_update);
	return ;
}

