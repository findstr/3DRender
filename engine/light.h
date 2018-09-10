#ifndef _LIGHT_H
#define _LIGHT_H

#include "rgb.h"
#include "mathlib.h"
#include "camera.h"

#define LIGHT_AMBIENT	(1)
#define LIGHT_DIRECTION	(2)
#define LIGHT_POINT	(3)
#define LIGHT_SPOT1	(4)
#define LIGHT_SPOT2	(5)


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
	struct light *next;
};

struct light* light_create();
void light_transform(struct camera *camera, struct object *obj);

#endif

