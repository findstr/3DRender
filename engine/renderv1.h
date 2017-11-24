#ifndef _RENDER_H
#define _RENDER_H

#include "shape.h"

#define RENDERLIST4D_MAX_POLYS	(32768)


struct renderlist4d {
	int state;
	int attr;
	struct polyf4d1 *poly_ptrs[RENDERLIST4D_MAX_POLYS];
	struct polyf4d1 poly_ptrs[RENDERLIST4D_MAX_POLYS];
	int num_polys;
};





#endif

