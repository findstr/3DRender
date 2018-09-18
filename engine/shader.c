#include "mathlib.h"
#include "texture.h"
#include "shader.h"

static float Time = 0.0f;
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
#if 1
	vector4_mul_matrix(&a2v->position, &G->MVP, &a2v->position);
	v2f->sv_position = a2v->position;
	v2f->texcoord0 = a2v->texcoord0;
#else
	vector4_t delta;
	vector4_t pos = a2v->position;
	double n = sin(time(NULL));
	vector4_init(&delta, 0.f, (0.5f - fabs(pos.x) / 50) * n , 0.f);
	vector4_add(&pos, &delta, &pos);
	vector4_mul_matrix(&pos, &G->MVP, &pos);
	v2f->sv_position = pos;
	v2f->texcoord0 = a2v->texcoord0;
	Time += 0.001f;
	Time -= floor(Time);
#endif
}

rgba_t
shader_default_diffuse_frag(struct shader_v2f *v2f, const struct shader_global *g)
{
	/*
	if (v2f->sv_position.x <= 0.0f)
		return RGBA(0x0, 0x0, 0x0, 0x0);
	if (v2f->sv_position.x >= g->SCREEN.x)
		return RGBA(0x0, 0x0, 0x0, 0x0);
	if (v2f->sv_position.y <= 0.0f)
		return RGBA(0x0, 0x0, 0x0, 0x0);
	if (v2f->sv_position.y >= g->SCREEN.y)
		return RGBA(0x0, 0x0, 0x0, 0x0);
		*/
	return texture_sample(g->BITMAP, &v2f->texcoord0);
}
