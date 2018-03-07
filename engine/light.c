#include <string.h>
#include <stdlib.h>
#include "mathlib.h"
#include "light.h"

/*
struct light {
	int type;
	rgba_t ambient;	//环境光强度
	rgba_t diffuse; //漫反射光强度
	rgba_t specular;//镜面反射光强度
	vector4_t pos;	//光源位置
	vector4_t dir;	//光源方向
	float kc, kl, kq;//衰减因子
	float spot_inner;//聚光灯内锥角
	float spot_outer;//聚光灯外锥角
	float pf;	 //聚光灯指数因子
};
*/
struct light *L;

struct light*
light_create()
{
	struct light *l = malloc(sizeof(*l));
	memset(l, 0, sizeof(*l));
#if 0
	l->type = LIGHT_AMBIENT;
	l->ambient = RGBA(255, 255, 10, 0);
#endif
#if 1
	l->type = LIGHT_DIRECTION;
	l->diffuse = RGBA(128, 200, 60, 0);
	vector4_init(&l->dir, 1, 0, 0);
#endif
#if 0
	l->type = LIGHT_POINT;
	vector4_init(&l->pos, 0, 10, 0);
	l->diffuse = RGBA(255, 255, 0, 0);
	l->kc = 0.0f;
	l->kl = 0.1f;
	l->kq = 0.0f;
#endif

	l->next = L;
	L = l;
	return l;
}

static inline void
light_tri(struct tri *p)
{
	int ii;
	struct light *l = L;
	vector4_t u,v,d;
	vertex_t *v0, *v1, *v2;
	vertex_t *vlist = p->vlist;
	unsigned int r_base[3], g_base[3], b_base[3],	//原来的颜色
		r_sum[3], g_sum[3], b_sum[3];	//全部光源的总体光照效果
	float dp,	//点积
		dist,	//表面和光源的距离
		i,	//强度
		nl,	//法线长度
		atten;	//衰减计算结果
	r_base[0] = RGBA_R(p->color[0]);
	g_base[0] = RGBA_G(p->color[0]);
	b_base[0] = RGBA_B(p->color[0]);

	r_base[1] = RGBA_R(p->color[1]);
	g_base[1] = RGBA_G(p->color[1]);
	b_base[1] = RGBA_B(p->color[1]);

	r_base[2] = RGBA_R(p->color[2]);
	g_base[2] = RGBA_G(p->color[2]);
	b_base[2] = RGBA_B(p->color[2]);


	v0 = &vlist[p->vert[0]];
	v1 = &vlist[p->vert[1]];
	v2 = &vlist[p->vert[2]];

	memset(r_sum, 0, sizeof(r_sum));
	memset(g_sum, 0, sizeof(g_sum));
	memset(b_sum, 0, sizeof(b_sum));
	while (l) {
		switch (l->type) {
		case LIGHT_AMBIENT:
			r_sum[0] += RGBA_R(l->ambient) * r_base[0] / 256;
			r_sum[1] += RGBA_R(l->ambient) * r_base[1] / 256;
			r_sum[2] += RGBA_R(l->ambient) * r_base[2] / 256;

			g_sum[0] += RGBA_G(l->ambient) * g_base[0] / 256;
			g_sum[1] += RGBA_G(l->ambient) * g_base[1] / 256;
			g_sum[2] += RGBA_G(l->ambient) * g_base[2] / 256;

			b_sum[0] += RGBA_B(l->ambient) * b_base[0] / 256;
			b_sum[1] += RGBA_B(l->ambient) * b_base[1] / 256;
			b_sum[2] += RGBA_B(l->ambient) * b_base[2] / 256;
			break;
		case LIGHT_DIRECTION:
			nl = vector4_magnitude(&v0->n);
			dp = vector4_dot(&v0->n, &l->dir);
			if (dp < 0) {
				dp = -dp;
				i = 128 * dp / nl;
				r_sum[0] += RGBA_R(l->diffuse) * r_base[0] * i / (256 * 128);
				g_sum[0] += RGBA_G(l->diffuse) * g_base[0] * i / (256 * 128);
				b_sum[0] += RGBA_B(l->diffuse) * b_base[0] * i / (256 * 128);
			}

			nl = vector4_magnitude(&v1->n);
			dp = vector4_dot(&v1->n, &l->dir);
			if (dp < 0) {
				dp = -dp;
				i = 128 * dp / nl;
				r_sum[1] += RGBA_R(l->diffuse) * r_base[1] * i / (256 * 128);
				g_sum[1] += RGBA_G(l->diffuse) * g_base[1] * i / (256 * 128);
				b_sum[1] += RGBA_B(l->diffuse) * b_base[1] * i / (256 * 128);
			}

			nl = vector4_magnitude(&v2->n);
			dp = vector4_dot(&v2->n, &l->dir);
			if (dp < 0) {
				dp = -dp;
				i = 128 * dp / nl;
				r_sum[2] += RGBA_R(l->diffuse) * r_base[2] * i / (256 * 128);
				g_sum[2] += RGBA_G(l->diffuse) * g_base[2] * i / (256 * 128);
				b_sum[2] += RGBA_B(l->diffuse) * b_base[2] * i / (256 * 128);
			}
			break;
		case LIGHT_POINT:
			nl = vector4_magnitude(&v0->n);
			vector4_sub(&v0->v, &l->pos, &d);
			dist = vector4_magnitude(&d);
			dp = vector4_dot(&v0->n, &d);
			if (dp < 0) {
				dp = -dp;
				atten = l->kc + l->kl * dist + l->kq * dist * dist;
				i = 128 * dp / (nl * dist* atten);
				r_sum[0] += RGBA_R(l->diffuse) * r_base[0] * i / (256 * 128);
				g_sum[0] += RGBA_G(l->diffuse) * g_base[0] * i / (256 * 128);
				b_sum[0] += RGBA_B(l->diffuse) * b_base[0] * i / (256 * 128);
			}


			nl = vector4_magnitude(&v1->n);
			vector4_sub(&v1->v, &l->pos, &d);
			dist = vector4_magnitude(&d);
			dp = vector4_dot(&v1->n, &d);
			if (dp < 0) {
				dp = -dp;
				atten = l->kc + l->kl * dist + l->kq * dist * dist;
				i = 128 * dp / (nl * dist* atten);
				r_sum[1] += RGBA_R(l->diffuse) * r_base[1] * i / (256 * 128);
				g_sum[1] += RGBA_G(l->diffuse) * g_base[1] * i / (256 * 128);
				b_sum[1] += RGBA_B(l->diffuse) * b_base[1] * i / (256 * 128);
			}

			nl = vector4_magnitude(&v2->n);
			vector4_sub(&v2->v, &l->pos, &d);
			dist = vector4_magnitude(&d);
			dp = vector4_dot(&v2->n, &d);
			if (dp < 0) {
				dp = -dp;
				atten = l->kc + l->kl * dist + l->kq * dist * dist;
				i = 128 * dp / (nl * dist* atten);
				r_sum[2] += RGBA_R(l->diffuse) * r_base[2] * i / (256 * 128);
				g_sum[2] += RGBA_G(l->diffuse) * g_base[2] * i / (256 * 128);
				b_sum[2] += RGBA_B(l->diffuse) * b_base[2] * i / (256 * 128);
			}

			break;
		case LIGHT_SPOT1:
			break;
		case LIGHT_SPOT2:
			break;
		}
		l = l->next;
	}
	for (ii = 0; ii < 3; ii++) {
		if (r_sum[ii] > 255)
			r_sum[ii] = 255;
		if (g_sum[ii] > 255)
			g_sum[ii] = 255;
		if (b_sum[ii] > 255)
			b_sum[ii] = 255;
		p->light[ii] = RGBA(r_sum[ii], g_sum[ii], b_sum[ii], 255);
	}
	return ;
}

void
light_transform(struct camera *camera, struct object *obj)
{
	struct tri *p = obj->rlist;

	while (p) {
		p->vlist = obj->vlist_trans;
		light_tri(p);
		p = p->next;
	}

}

