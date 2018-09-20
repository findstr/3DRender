#ifndef _SHADER_H
#define _SHADER_H

#include "rgb.h"
#include "mathlib.h"
#include "texture.h"

struct shader_global {
	float TIME;
	matrix_t  MVP;
	vector2_t SCREEN;
	struct texture *BITMAP;
};

struct shader_appdata {
	vector4_t position;
	vector4_t normal;
	vector2_t  texcoord0;
};

struct shader_v2f {
	vector4_t sv_position;
	vector2_t texcoord0;
};

typedef void (shader_vert_t)(struct shader_appdata *a2v, struct shader_v2f *v2f, const struct shader_global *g);
typedef rgba_t (shader_frag_t)(struct shader_v2f *v2f, const struct shader_global *g);

struct shader_routine {
	shader_vert_t *vert;
	shader_frag_t *frag;
};

void shader_v2f_lerp(const struct shader_v2f *from, const struct shader_v2f *to, float t, struct shader_v2f *res);
void shader_default_diffuse_vert(struct shader_appdata *a2v, struct shader_v2f *v2f, const struct shader_global *g);
rgba_t shader_default_diffuse_frag(struct shader_v2f *a2v, const struct shader_global *g);

void shader_anim_vert(struct shader_appdata *a2v, struct shader_v2f *v2f, const struct shader_global *g);
rgba_t shader_anim_frag(struct shader_v2f *a2v, const struct shader_global *g);






#endif
