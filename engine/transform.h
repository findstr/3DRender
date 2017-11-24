#ifndef _RENDER_H
#define _RENDER_H

#include "mathlib.h"
#include "primitive.h"


enum transform_sel {
	TRANSFORM_LOCAL_ONLY = 1,
	TRANSFORM_TRANS_ONLY = 2,
	TRANSFORM_LOCAL_TRANS = 3,
};

void transform_render(struct render4d *list,
		matrix_t *mt, enum transform_sel sel);

void transform_obj(struct object4d *obj,matrix_t *mt,
		enum transform_sel sel, int transform_basis);





#endif

