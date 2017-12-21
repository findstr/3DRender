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
	l->ambient = RGBA(255, 255, 255, 0);
#endif
#if 0
	l->type = LIGHT_DIRECTION;
	l->diffuse = RGBA(255, 255, 255, 0);
	vector4_init(&l->dir, 1, 0, 0);
#endif
	l->type = LIGHT_POINT;
	vector4_init(&l->pos, 0, 10, 0);
	l->diffuse = RGBA(255, 255, 0, 0);
	l->kc = 0.0f;
	l->kl = 0.1f;
	l->kq = 0.0f;


	l->next = L;
	L = l;
	return l;
}

static inline void
light_tri(struct tri *p)
{
	struct light *l = L;
	vector4_t u,v,n,d;
	int v0, v1, v2;
	unsigned int r_base, g_base, b_base,	//原来的颜色
		r_sum, g_sum, b_sum,		//全部光源的总体光照效果
		shaded_color;			//最后的颜色
	float dp,	//点积
		dist,	//表面和光源的距离
		i,	//强度
		nl,	//法线长度
		atten;	//衰减计算结果
	r_base = RGBA_R(p->color);
	g_base = RGBA_G(p->color);
	b_base = RGBA_B(p->color);
	v0 = p->vert[0];
	v1 = p->vert[1];
	v2 = p->vert[2];

	r_sum = 0;
	g_sum = 0;
	b_sum = 0;
	while (l) {
		switch (l->type) {
		case LIGHT_AMBIENT:
			r_sum += RGBA_R(l->ambient) * r_base / 256;
			g_sum += RGBA_G(l->ambient) * g_base / 256;
			b_sum += RGBA_B(l->ambient) * b_base / 256;
			break;
		case LIGHT_DIRECTION:
			vector4_sub(&p->vlist[v1], &p->vlist[v0], &u);
			vector4_sub(&p->vlist[v2], &p->vlist[v0], &v);
			vector4_cross(&u, &v, &n);
			nl = vector4_magnitude(&n);
			dp = vector4_dot(&n, &l->dir);
			if (dp >= 0)
				break;
			dp = -dp;
			i = 128 * dp / nl;
			r_sum += RGBA_R(l->diffuse) * r_base * i / (256 * 128);
			g_sum += RGBA_G(l->diffuse) * g_base * i / (256 * 128);
			b_sum += RGBA_B(l->diffuse) * b_base * i / (256 * 128);
			break;
		case LIGHT_POINT:
			vector4_sub(&p->vlist[v1], &p->vlist[v0], &u);
			vector4_sub(&p->vlist[v2], &p->vlist[v0], &v);
			vector4_cross(&u, &v, &n);
			nl = vector4_magnitude(&n);
			vector4_sub(&p->vlist[v0], &l->pos, &d);
			dist = vector4_magnitude(&d);
			dp = vector4_dot(&n, &d);
			if (dp >= 0)
				break;
			dp = -dp;
			atten = l->kc + l->kl * dist + l->kq * dist * dist;
			i = 128 * dp / (nl * dist* atten);
			r_sum += RGBA_R(l->diffuse) * r_base * i / (256 * 128);
			g_sum += RGBA_G(l->diffuse) * g_base * i / (256 * 128);
			b_sum += RGBA_B(l->diffuse) * b_base * i / (256 * 128);
			break;
		case LIGHT_SPOT1:
			break;
		case LIGHT_SPOT2:
			break;
		}
		l = l->next;
	}
	if (r_sum > 255)
		r_sum = 255;
	if (g_sum > 255)
		g_sum = 255;
	if (b_sum > 255)
		b_sum = 255;
	p->light = RGBA(r_sum, g_sum, b_sum, 255);
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

