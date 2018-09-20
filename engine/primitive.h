#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "mathlib.h"
#include "matrial.h"
#include "rgb.h"

typedef struct transform {
	vector3_t pos;
	vector3_t scale;
	quaternion_t rot;
} transform_t;

typedef struct vertex {
	vector3_t v;	//vertex
	vector3_t n;	//normal
	vector2_t t;
	struct shader_appdata  app;
	struct shader_v2f v2f;
} vertex_t;

struct tri {
	int state;
	int attr;
	int vert[3];
	rgba_t color[3];
	rgba_t light[3];	//顶点颜色
	vector4_t normal_local;
	vector4_t normal;	//
	float avg_z;
	vertex_t *vlist;
	struct tri *next;
};

struct object {
	int id;
	int state;
	char name[64];
	float radius_avg;
	float radius_max;
	transform_t transform;
	int vertices_num;
	int tri_num;
	vertex_t *vlist;
	struct tri *plist;

	struct matrial martial;
	struct tri *rlist;
	struct object *next;
};

#define RENDERLIST4D_MAX_POLYS	(32768)


#define POLY4D_ATTR_2SIDED		(1 << 0)
#define POLY4D_ATTR_TRANSPARENT		(1 << 2)
#define POLY4D_ATTR_RGB24		(1 << 4)

#define POLY4D_ATTR_SHADE_MODE_PURE	0x0020
#define POLY4D_ATTR_SHADE_MODE_FLAT	0x0040
#define POLY4D_ATTR_SHADE_MODE_GOURAUD	0x0080
#define POLY4D_ATTR_SHADE_MODE_PHONG	0x0100

#define POLY4D_STATE_ACTIVE		(1 << 0)
#define POLY4D_STATE_CLIPPED		(1 << 1)
#define POLY4D_STATE_BACKFACE		(1 << 2)

#define OBJECT4D_STATE_ACTIVE		0x0001
#define OBJECT4D_STATE_VISIBLE		0x0002
#define OBJECT4D_STATE_CULLED		0x0004


void object_init(struct object *obj);
void object_finalize(struct object *obj);
void object_polynormals(struct object *obj);
void object_vertexnormals(struct object *obj);



#endif

