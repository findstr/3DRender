#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "rgb.h"
#include "plg.h"

#define DBG_PRINT	printf
//void DBG_PRINT(const char *p, ...) {}

#define PLX_RGB_MASK          0x8000
#define PLX_SHADE_MODE_MASK   0x6000
#define PLX_2SIDED_MASK       0x1000
#define PLX_COLOR_MASK        0x0fff


#define PLX_COLOR_MODE_RGB_FLAG     0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000

#define PLX_2SIDED_FLAG              0x1000
#define PLX_1SIDED_FLAG              0x0000

#define PLX_SHADE_MODE_PURE_FLAG      0x0000
#define PLX_SHADE_MODE_CONSTANT_FLAG  0x0000
#define PLX_SHADE_MODE_FLAT_FLAG      0x2000
#define PLX_SHADE_MODE_GOURAUD_FLAG   0x4000
#define PLX_SHADE_MODE_PHONG_FLAG     0x6000
#define PLX_SHADE_MODE_FASTPHONG_FLAG 0x6000

static char *
nextline(FILE *fp, char *buff, int sz)
{
	for (;;) {
		int i = 0;
		int n = 0;
		if (!fgets(buff, sz, fp))
			return NULL;
		while (isspace(buff[i]))
			i++;
		n = strlen(buff);
		if (i >= n || buff[i] == '#')
			continue;
		return &buff[i];
	}
}

static float
compute_radius(struct object *obj)
{
	int i;
	obj->radius_avg = 0;
	obj->radius_max = 0;
	for (i = 0; i < obj->vertices_num; i++) {
		float x = obj->vlist_local[i].v.x;
		float y = obj->vlist_local[i].v.y;
		float z = obj->vlist_local[i].v.z;
		float dis = (float)sqrt(x * x + y * y + z * z);
		obj->radius_avg += dis;
		if (dis > obj->radius_max)
			obj->radius_max = dis;
	}
	obj->radius_avg /= obj->vertices_num;
	return obj->radius_max;
}

int plg_load(struct object *obj, const char *filename,
		const vector4_t *scale, const vector4_t *pos,
		const vector4_t *rot)
{
	int i;
	FILE *fp;
	char *str;
	char buff[256];
	printf("load:%s\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL)
		return -1;
	str = nextline(fp, buff, sizeof(buff));
	if (str == NULL)
		return -1;
	memset(obj, 0, sizeof(*obj));
	DBG_PRINT("Object Descriptor: %s\n", str);
	sscanf(str, "%s %d %d", obj->name, &obj->vertices_num, &obj->tri_num);
	object_init(obj);
	for (i = 0; i < obj->vertices_num; i++) {
		char xa[10], ya[10], za[10], ua[10], va[10];
		str = nextline(fp, buff, sizeof(buff));
		if (str == NULL) {
			DBG_PRINT("PLG file error with file %s (vertex list invalid).",filename);
			return -1;
		}
		sscanf(str, "%s %s %s %s %s", xa, ya, za, ua, va);
		DBG_PRINT("%s", str);
		obj->vlist_local[i].v.x = atoi(xa);
                obj->vlist_local[i].v.y = atoi(ya);
		obj->vlist_local[i].v.z = atoi(za);
		obj->vlist_local[i].v.w = 1;
		obj->vlist_local[i].t.x = atof(ua);
		obj->vlist_local[i].t.y = atof(va);

		obj->vlist_local[i].v.x*=scale->x;
		obj->vlist_local[i].v.y*=scale->y;
		obj->vlist_local[i].v.z*=scale->z;

		DBG_PRINT("\nVertex %d = %f, %f, %f, %f %f %f\n", i,
                                           obj->vlist_local[i].v.x,
                                           obj->vlist_local[i].v.y,
                                           obj->vlist_local[i].v.z,
                                           obj->vlist_local[i].v.w,
                                           obj->vlist_local[i].t.x,
                                           obj->vlist_local[i].t.y
					   );
	}
	compute_radius(obj);
	DBG_PRINT("\nObject average radius = %f, max radius = %f",
            obj->radius_avg, obj->radius_max);

	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts    = 0; // number of vertices for current poly (always 3)
	char tmp_string[8];        // temp string to hold surface descriptor in and
				   // test if it need to be converted from hex
	for (i = 0; i < obj->tri_num; i++) {
		str = nextline(fp, buff, sizeof(buff));
		if (str == NULL) {
			DBG_PRINT("PLG file error with file %s (polygon descriptor invalid).",filename);
			return -1;
		}
		DBG_PRINT("\nPolygon %d:", i);
		int *vert = obj->plist[i].vert;
		sscanf(str, "%s %d %d %d %d", tmp_string, &poly_num_verts,
				&vert[0], &vert[1],&vert[2]);
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			sscanf(tmp_string,"%x", &poly_surface_desc);
		else
			poly_surface_desc = atoi(tmp_string);
		obj->plist[i].vlist = obj->vlist_local;
		DBG_PRINT("\nSurface Desc = 0x%.4x, num_verts = %d, vert_indices [%d, %d, %d]",
				 poly_surface_desc,
				 poly_num_verts,
				 obj->plist[i].vert[0],
				 obj->plist[i].vert[1],
				 obj->plist[i].vert[2]);
		if ((poly_surface_desc & PLX_2SIDED_FLAG)) {
			obj->plist[i].attr |= POLY4D_ATTR_2SIDED;
			DBG_PRINT("\n2 sided.");
		} else {
			DBG_PRINT("\n1 sided.");
		}
		if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG)) {
			obj->plist[i].color[0] = RGBA(255, 0, 0, 255);
			obj->plist[i].color[1] = RGBA(0, 255, 0, 255);
			obj->plist[i].color[2] = RGBA(0, 0, 255, 255);
		} else {
			assert(0);
		}

		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);
		switch(shade_mode) {
		case PLX_SHADE_MODE_PURE_FLAG:
			obj->plist[i].attr |=  POLY4D_ATTR_SHADE_MODE_PURE;
			DBG_PRINT("\nShade mode = pure");
			break;
		case PLX_SHADE_MODE_FLAT_FLAG:
			obj->plist[i].attr |= POLY4D_ATTR_SHADE_MODE_FLAT;
			DBG_PRINT("\nShade mode = flat");
			break;
		case PLX_SHADE_MODE_GOURAUD_FLAG:
			obj->plist[i].attr |= POLY4D_ATTR_SHADE_MODE_GOURAUD;
			DBG_PRINT("\nShade mode = gouraud");
			break;
		case PLX_SHADE_MODE_PHONG_FLAG:
			obj->plist[i].attr |= POLY4D_ATTR_SHADE_MODE_PHONG;
			DBG_PRINT("\nShade mode = phong");
			break;
		default: break;
		}
		obj->plist[i].state = POLY4D_STATE_ACTIVE;
	}
	obj->state = OBJECT4D_STATE_ACTIVE | OBJECT4D_STATE_VISIBLE;
	obj->transform.pos = *pos;
	obj->transform.rot = IQUATERNION;
	object_polynormals(obj);
	object_vertexnormals(obj);
	fclose(fp);
	return 0;
}


