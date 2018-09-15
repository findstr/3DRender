#include "mathlib.h"
#include "bitmap.h"
#include "shader.h"

static rgba_t
tex2D(struct bitmap *BITMAP, vector2_t *uv)
{
	int x = uv->x * (BITMAP->info.width - 1);
	int y = uv->y * (BITMAP->info.height - 1);
	if (y >= BITMAP->info.height)
		y = BITMAP->info.height - 1;
	return BITMAP->buffer[y * BITMAP->info.width + x];
}

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
	return tex2D(g->BITMAP, &v2f->texcoord0);
}
