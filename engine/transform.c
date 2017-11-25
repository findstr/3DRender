#include <assert.h>
#include <stddef.h>
#include "primitive.h"
#include "mathlib.h"
#include "transform.h"

static void
select_idx(enum transform_sel sel, size_t *src, size_t *dst)
{
	switch (sel) {
	case TRANSFORM_LOCAL_ONLY:
		*src = 0;
		*dst = 0;
		break;
	case TRANSFORM_TRANS_ONLY:
		*src = 1;
		*dst = 1;
		break;
	case TRANSFORM_LOCAL_TRANS:
		*src = 0;
		*dst = 1;
		break;
	default:
		assert(0);
	}
	return ;
}

static inline size_t
polyf4d_offset(size_t src)
{
	size_t off;
	if (src == 0)
		off = offsetof(struct polyf4d, vlist_local);
	else
		off = offsetof(struct polyf4d, vlist_trans);
	return off;
}

static inline size_t
object4d_offset(size_t src)
{
	size_t off;
	if (src == 0)
		off = offsetof(struct object4d, vlist_local);
	else
		off = offsetof(struct object4d, vlist_trans);
	return off;
}

void
transform_render(struct render4d *list, matrix_t *mt,
		enum transform_sel sel)
{
	int i,j;
	size_t offset_src, offset_dst;
	select_idx(sel, &offset_src, &offset_dst);
	offset_src = polyf4d_offset(offset_src);
	offset_dst = polyf4d_offset(offset_dst);
	for (i = 0; i < list->num_polys; i++) {
		struct polyf4d *p = list->poly_ptrs[i];
		if (p == NULL)
			continue;
		if ((p->state & POLY4D_STATE_ACTIVE) == 0)
			continue;
		if ((p->state & POLY4D_STATE_CLIPPED) == 0)
			continue;
		if ((p->state & POLY4D_STATE_BACKFACE) == 0)
			continue;
		for (j = 0; j < 3; j++) {
			vector4_t *s = (vector4_t *)((char *)p + offset_src);
			vector4_t *d = (vector4_t *)((char *)p + offset_dst);
			vector4_mul_matrix(s, mt, d);
		}
	}
}

void transform_obj(struct object4d *obj,matrix_t *mt,
		enum transform_sel sel, int transform_basis)
{
	int i;
	size_t offset_src, offset_dst;
	select_idx(sel, &offset_src, &offset_dst);
	offset_src = object4d_offset(offset_src);
	offset_dst = object4d_offset(offset_dst);
	for (i = 0; i < obj->vertices_num; i++) {
		vector4_t *src = (vector4_t *)((char *)obj + offset_src);
		vector4_t *dst = (vector4_t *)((char *)obj + offset_dst);
		vector4_mul_matrix(&src[i], mt, &dst[i]);
	}
	if (transform_basis) {
		vector4_mul_matrix(&obj->ux, mt, &obj->ux);
		vector4_mul_matrix(&obj->uy, mt, &obj->uy);
		vector4_mul_matrix(&obj->uz, mt, &obj->uz);
	}
	return ;
}


void
transform_model2world(struct object4d *obj, enum transform_sel sel)
{
	int i;
	size_t offsrc;
	size_t offdst;
	size_t offset_src, offset_dst;
	select_idx(sel, &offset_src, &offset_dst);
	offset_src = object4d_offset(offset_src);
	offset_dst = object4d_offset(offset_dst);
	vector4_t *src = (vector4_t *)((char *)obj + offset_src);
	vector4_t *dst = (vector4_t *)((char *)obj + offset_dst);
	struct object4d *n = 0;
	for (i = 0; i < obj->vertices_num; i++)
		vector4_add(&src[i], &obj->world_pos, &dst[i]);
	return ;
}

void
transform_resetobj(struct object4d *obj)
{
	int i;
	obj->state &= ~OBJECT4D_STATE_CULLED;
	for (i = 0; i < obj->polys_num; i++) {
		struct poly4d *p = &obj->plist[i];
		if (!(p->state & POLY4D_STATE_ACTIVE))
			continue;
		p->state &= ~POLY4D_STATE_CLIPPED;
		p->state &= ~POLY4D_STATE_BACKFACE;
	}
	return ;
}





