#include <stdlib.h>
#include <string.h>
#include "mathlib.h"
#include "primitive.h"

void
object_init(struct object *obj)
{
	size_t sz;
	sz = obj->vertices_num * sizeof(vertex_t);
	obj->vlist = malloc(sz);
	memset(obj->vlist, 0, sz);
	sz = obj->tri_num * sizeof(struct tri);
	obj->plist = malloc(sz);
	memset(obj->plist, 0, sz);
	return ;
}

void
object_finalize(struct object *obj)
{
	free(obj->vlist);
	free(obj->plist);
	return ;
}

void
object_polynormals(struct object *obj)
{
	int i;
	int v0, v1, v2;
	vector3_t u,v,n;
	vertex_t *vlist = obj->vlist;
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];
		vector3_sub(&vlist[v1].v, &vlist[v0].v, &u);
		vector3_sub(&vlist[v2].v, &vlist[v0].v, &v);
		vector3_cross(&u, &v, &n);
		vector4_init(&p->normal_local, n.x, n.y, n.z);
		p->nlength = vector3_magnitude(&n);
	}
	return ;
}

void
object_vertexnormals(struct object *obj)
{
#if 1
	int i, v0, v1, v2 ;
	vector3_t u,v,n;
	vertex_t *vlist = obj->vlist;
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];

		vector3_sub(&vlist[v1].v, &vlist[v0].v, &u);
		vector3_sub(&vlist[v2].v, &vlist[v0].v, &v);
		vector3_cross(&u, &v, &n);
		vector3_add(&vlist[v0].n, &n, &vlist[v0].n);
		vector3_add(&vlist[v1].n, &n, &vlist[v1].n);
		vector3_add(&vlist[v2].n, &n, &vlist[v2].n);
		vlist[v0].v2f.sv_position.w += 1;
	}
	for (i = 0; i < obj->vertices_num; i++) {
		vertex_t *v = &vlist[i];
		float w = v->v2f.sv_position.w;
		v->n.x /= w;
		v->n.y /= w;
		v->n.z /= w;
		v->v2f.sv_position.w = 1.0f;
	}
#endif
	return ;
}




