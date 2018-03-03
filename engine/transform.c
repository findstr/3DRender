#include <assert.h>
#include <stddef.h>
#include "primitive.h"
#include "mathlib.h"
#include "transform.h"


void
transform_obj(struct object *obj,matrix_t *mt, int transform_basis)
{
	int i;
	vertex_t *list = obj->vlist_trans;
	for (i = 0; i < obj->vertices_num; i++)
		vector4_mul_matrix(&list[i].v, mt, &list[i].v);
	if (transform_basis) {
		vector4_mul_matrix(&obj->ux, mt, &obj->ux);
		vector4_mul_matrix(&obj->uy, mt, &obj->uy);
		vector4_mul_matrix(&obj->uz, mt, &obj->uz);
	}
	return ;
}

