#ifndef _RENDER_H
#define _RENDER_H

#include "mathlib.h"
#include "primitive.h"

void transform_world(struct object *obj);
void transform_obj(struct object *obj,matrix_t *mt, int transform_basis);


#endif

