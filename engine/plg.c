#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "plg.h"

#define DBG_PRINT	printf

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
		float x = obj->vlist_local[i].x;
		float y = obj->vlist_local[i].y;
		float z = obj->vlist_local[i].z;
		float dis = sqrt(x * x + y * y + z * z);
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
	memset(obj, 0, sizeof(*obj));
	obj->state = OBJECT4D_STATE_ACTIVE | OBJECT4D_STATE_VISIBLE;
	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;
	obj->world_pos.w = pos->w;
	fp = fopen(filename, "r");
	if (fp == NULL)
		return -1;
	str = nextline(fp, buff, sizeof(buff));
	if (str == NULL)
		return -1;
	DBG_PRINT("Object Descriptor: %s\n", str);

	sscanf(str, "%s %d %d", obj->name, &obj->vertices_num, &obj->polys_num);
	for (i = 0; i < obj->vertices_num; i++) {
		char xa[10], ya[10], za[10];
		str = nextline(fp, buff, sizeof(buff));
		if (str == NULL) {
			DBG_PRINT("PLG file error with file %s (vertex list invalid).",filename);
			return -1;
		}
		DBG_PRINT("%s", str);
		sscanf(str, "%s %s %s", xa, ya, za);
		obj->vlist_local[i].x = atoi(xa);
                obj->vlist_local[i].y = atoi(ya);
		obj->vlist_local[i].z = atoi(za);
		obj->vlist_local[i].w = 1;

		obj->vlist_local[i].x*=scale->x;
		obj->vlist_local[i].y*=scale->y;
		obj->vlist_local[i].z*=scale->z;

		DBG_PRINT("%s-%s %f-%f\n", xa, ya, obj->vlist_local[i].x,
                scale->x);


		DBG_PRINT("\nVertex %d = %f, %f, %f, %f\n", i,
                                           obj->vlist_local[i].x,
                                           obj->vlist_local[i].y,
                                           obj->vlist_local[i].z,
                                           obj->vlist_local[i].w);
	}
	compute_radius(obj);
	DBG_PRINT("\nObject average radius = %f, max radius = %f",
            obj->radius_avg, obj->radius_max);

	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts    = 0; // number of vertices for current poly (always 3)
	char tmp_string[8];        // temp string to hold surface descriptor in and
				   // test if it need to be converted from hex
	for (i = 0; i < obj->polys_num; i++) {
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
			int color = poly_surface_desc & 0xfff;
			obj->plist[i].color = color;
			DBG_PRINT("\nRGB color = [%x]", color);
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
	fclose(fp);
	return 0;
}


