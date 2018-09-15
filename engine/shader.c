#include "mathlib.h"
#include "texture.h"
#include "shader.h"

void
shader_v2f_lerp(const struct shader_v2f *from, const struct shader_v2f *to,
	float t, struct shader_v2f *res)
{
	vector4_lerp(&from->sv_position, &to->sv_position, t, &res->sv_position);
	vector2_lerp(&from->texcoord0, &to->texcoord0, t, &res->texcoord0);
}

void
shader_default_diffuse_vert(struct shader_appdata *a2v,
	struct shader_v2f *v2f,
	const struct shader_global *G)
{
	vector4_mul_matrix(&a2v->position, &G->MVP, &a2v->position);
	v2f->sv_position = a2v->position;
	v2f->texcoord0 = a2v->texcoord0;
}

rgba_t
shader_default_diffuse_frag(struct shader_v2f *v2f, const struct shader_global *g)
{
	return texture_sample(g->BITMAP, &v2f->texcoord0);
}
