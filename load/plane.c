#include <assert.h>
#include <stdio.h>
#include "mathlib.h"
#include "plane.h"

#define WIDTH			(10.0f)
#define VERTEX_LINE		(10)
#define DELTA			(WIDTH / VERTEX_LINE)
#define VERTEX_COUNT		((VERTEX_LINE + 1) * (VERTEX_LINE + 1))
#define VERTEX_LOC(x, z)	((VERTEX_LINE + 1) * (z) + (x))
#define TRI_COUNT		(VERTEX_LINE * VERTEX_LINE * 2)

int
plane_load(struct object *obj, const vector4_t *pos, const quaternion_t *rot)
{
	float u, v;
	vertex_t *vertex;
	struct tri *tri;
	int x, z, idx, zo;
	float xx, zz;
	vertex = (vertex_t *)malloc(VERTEX_COUNT * sizeof(*vertex));
	tri = (struct tri *)malloc(TRI_COUNT * sizeof(*tri));
	v = 0.f;
	assert(VERTEX_LINE % 2 == 0);
	zz = - WIDTH / 2;
	for (z = 0; z <= VERTEX_LINE; z++) {
		u = 0.f;
		idx = z * (VERTEX_LINE + 1);
		xx = -WIDTH / 2;
		for (x = 0; x <= VERTEX_LINE; x++) {
			vertex_t *ver;
			ver = &vertex[idx + x];
			vector4_init(&ver->v, xx, 0.0, zz);
			vector4_init(&ver->n, 0.f, 1.f, 0.f);
			vector2_init(&ver->t, u, v);
			assert(u < 1.0f);
			assert(v < 1.0f);
			u += 1.0f / (VERTEX_LINE + 1);
			xx += DELTA;
		}
		printf("\n");
		v += 1.0f / (VERTEX_LINE + 1);
		zz += DELTA;
	}
	zo = 0;
	idx = 0;
	for (z = 0; z < VERTEX_LINE; z++) {
		for (x = 0; x < VERTEX_LINE; x++) {
			tri[idx].vert[0] = VERTEX_LOC(x + 0, zo + 0);
			tri[idx].vert[1] = VERTEX_LOC(x + 1, zo + 0);
			tri[idx].vert[2] = VERTEX_LOC(x + 0, zo + 1);
			idx += 1;
			tri[idx].vert[0] = VERTEX_LOC(x + 1, zo + 0);
			tri[idx].vert[1] = VERTEX_LOC(x + 1, zo + 1);
			tri[idx].vert[2] = VERTEX_LOC(x + 0, zo + 1);
			idx += 1;
		}
		++zo;
	}
	obj->vlist = vertex;
	obj->vertices_num = VERTEX_COUNT;
	obj->plist = tri;
	obj->tri_num = TRI_COUNT;
	obj->state = OBJECT4D_STATE_ACTIVE | OBJECT4D_STATE_VISIBLE;
	obj->transform.pos = *pos;
	obj->transform.rot = *rot;
	object_polynormals(obj);
	object_vertexnormals(obj);
	return 0;
}
