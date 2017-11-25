#include <assert.h>
#include <stddef.h>
#include "primitive.h"
#include "mathlib.h"
#include "transform.h"


void
transform_obj(struct object *obj,matrix_t *mt, int transform_basis)
{
	int i;
	vector4_t *list = obj->vlist_trans;
	for (i = 0; i < obj->vertices_num; i++)
		vector4_mul_matrix(&list[i], mt, &list[i]);
	if (transform_basis) {
		vector4_mul_matrix(&obj->ux, mt, &obj->ux);
		vector4_mul_matrix(&obj->uy, mt, &obj->uy);
		vector4_mul_matrix(&obj->uz, mt, &obj->uz);
	}
	return ;
}


void
transform_resetobj(struct object *obj)
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





