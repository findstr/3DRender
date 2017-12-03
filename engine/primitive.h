#ifndef _SHAPE_H
#define _SHAPE_H

#include "mathlib.h"

typedef int color_t;

typedef struct transform {
	vector4_t pos;
	quaternion_t rot;
} transform_t;

struct poly1 {
	vector3_t point[3];
};

struct poly2 {
	vector3_t *point;
	int vertices[3];
};

struct tri {
	int state;
	int attr;
	int vert[3];
	color_t color;
	vector4_t *vlist;
	struct tri *next;
};

struct polyf4d {
	int state;
	int attr;
	int color;

	vector4_t vlist_local[3];
	vector4_t vlist_trans[3];
};

struct object {
	int id;
	char name[64];
	int state;
	int attr;
	float radius_avg;
	float radius_max;

	transform_t transform;

	vector4_t ux,uy,uz;

	int vertices_num;
	vector4_t vlist_local[64];
	vector4_t *vlist_trans;

	int polys_num;
	struct tri plist[128];
	struct tri *rlist;
	struct object *next;
};

#define RENDERLIST4D_MAX_POLYS	(32768)

struct render4d {
	int state;
	int attr;
	struct polyf4d *poly_ptrs[RENDERLIST4D_MAX_POLYS];
	struct polyf4d polys[RENDERLIST4D_MAX_POLYS];
	int num_polys;
};


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

#endif

