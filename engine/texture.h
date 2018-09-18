#ifndef _TEXTURE_H
#define _TEXTURE_H
#include <stdint.h>
#include "mathlib.h"
#include "rgb.h"

struct texture {
	int width;
	int height;
	rgba_t *color;
};

static inline rgba_t
texture_sample(struct texture *t, vector2_t *uv)
{
	int x = (int)(uv->x * (t->width - 1));
	int y = (int)(uv->y * (t->height - 1));
	return t->color[y * t->width + x];

}


#endif

