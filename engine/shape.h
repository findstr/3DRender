#ifndef _SHAPE_H
#define _SHAPE_H

#include "mathlib.h"

typedef int color_t;

struct poly1 {
	vector3_t point[3];
};

struct poly2 {
	vector3_t *point;
	int vertices[3];
};

struct poly4d {
	int state;
	int attr;
	color_t color;
	vector4_t *vlist;
	int vert[3];
};

struct polyf4d {
	int state;
	int attr;
	int color;

	vector4_t vlist[3];
	vector4_t tvlist[3];

	struct polyf4d *next;
	struct polyf4d *prev;
};

struct object4d {
	int id;
	char name[64];
	int state;
	int attr;
	float radius_avg;
	float radius_max;

	vector4_t world_pos;
	vector4_t dir;

	vector4_t ux,uy,uz;

	int vertices_num;
	vector4_t vlist_local[64];
	vector4_t vlist_trans[64];

	int polys_num;
	struct poly4d plist[128];
};

#define POLY4D_ATTR_2SIDED		(1 << 0)
#define POLY4D_ATTR_TRANSPARENT		(1 << 2)
#define POLY4D_ATTR_RGB24		(1 << 4)

#define POLY4D_ATTR_SHADE_MODE_PURE	0x0020
#define POLY4D_ATTR_SHADE_MODE_FLAT	0x0040
#define POLY4D_ATTR_SHADE_MODE_GOURAUD	0x0080
#define POLY4D_ATTR_SHADE_MODE_PHONG	0x0100

#define POLY4D_STATE_ACTIVE		0x0001
#define POLY4D_STATE_CLIPPED		0x0002
#define POLY4D_STATE_BACKFACE		0x0004

#define OBJECT4D_STATE_ACTIVE		0x0001
#define OBJECT4D_STATE_VISIBLE		0x0002
#define OBJECT4D_STATE_CULLED		0x0004




#endif

