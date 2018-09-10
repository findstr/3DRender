#include <stdlib.h>
#include <string.h>
#include "primitive.h"


void
vertex_lerp(const vertex_t *a, const vertex_t *b, float t, vertex_t *c)
{
	vector4_lerp(&a->v, &b->v, t, &c->v);
	vector2_lerp(&a->t, &b->t, t, &c->t);
}

void
object_init(struct object *obj)
{
	size_t sz;
	sz = obj->vertices_num * sizeof(vertex_t);
	obj->vlist_local = malloc(sz);
	obj->vlist_trans = malloc(sz);
	memset(obj->vlist_local, 0, sz);
	memset(obj->vlist_trans, 0, sz);
	sz = obj->tri_num * sizeof(struct tri);
	obj->plist = malloc(sz);
	memset(obj->plist, 0, sz);
	return ;
}

void
object_finalize(struct object *obj)
{
	free(obj->vlist_local);
	free(obj->vlist_trans);
	free(obj->plist);
	return ;
}

void
object_polynormals(struct object *obj)
{
	int i;
	float nl;
	int v0, v1, v2;
	vector4_t u,v,n,d;
	vertex_t *vlist = obj->vlist_local;
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];
		vector4_sub(&vlist[v1].v, &vlist[v0].v, &u);
		vector4_sub(&vlist[v2].v, &vlist[v0].v, &v);
		vector4_cross(&u, &v, &n);
		p->normal_local = n;
		p->nlength = vector4_magnitude(&n);
	}
	return ;
}

void
object_vertexnormals(struct object *obj)
{
	int i, v0, v1, v2 ;
	vector4_t u,v,n;
	vertex_t *vlist = obj->vlist_local;
	for (i = 0; i < obj->tri_num; i++) {
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];

		vector4_sub(&vlist[v1].v, &vlist[v0].v, &u);
		vector4_sub(&vlist[v2].v, &vlist[v0].v, &v);
		vector4_cross(&u, &v, &n);
		vector4_add(&vlist[v0].n, &n, &vlist[v0].n);
		vector4_add(&vlist[v1].n, &n, &vlist[v1].n);
		vector4_add(&vlist[v2].n, &n, &vlist[v2].n);
		vlist[v0].n.w += 1;
	}
	for (i = 0; i < obj->vertices_num; i++) {
		vertex_t *v = &vlist[i];
		v->n.x /= v->n.w;
		v->n.y /= v->n.w;
		v->n.z /= v->n.w;
		v->n.w = 1.0f;
	}
	return ;
}




