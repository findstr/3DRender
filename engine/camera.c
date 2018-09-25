#include <math.h>
#include <assert.h>
#include "mathlib.h"
#include "driver.h"
#include "primitive.h"
#include "camera.h"

void
camera_init(struct camera *cam, vector3_t *pos,
		vector3_t *dir, vector3_t *target,
		float near_clip_z, float far_clip_z, float fov,
		float viewport_width, float viewport_height)
{
	cam->pos = *pos;
	cam->dir = *dir;
	cam->dir.z = -cam->dir.z;
	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;
	cam->viewport_width = viewport_width;
	cam->viewport_height = viewport_height;
	cam->aspect_ratio = viewport_width / viewport_height;
	cam->V = IMATRIX;
	cam->fov = fov;
	return ;
}

void
camera_move(struct camera *cam, const vector3_t *add)
{
	vector3_add(&cam->pos, add, &cam->pos);
	return ;
}


static inline void
camera_matrix(struct camera *cam,
		matrix_t *mt_inv,
		matrix_t *mx_inv,
		matrix_t *my_inv,
		matrix_t *mz_inv)
{
	float theta_x, theta_y, theta_z;
	*mt_inv = (matrix_t){
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1.0f
	};

	theta_x = -cam->dir.x;
	theta_y = -cam->dir.y;
	theta_z = -cam->dir.z;

	matrix_rotate_x(mx_inv, cam->dir.x);
	matrix_rotate_y(my_inv, cam->dir.y);
	matrix_rotate_z(mz_inv, cam->dir.z);

#if 0
	matrix_print("mt", mx_inv);
	matrix_print("mx", mx_inv);
	matrix_print("my", my_inv);
	matrix_print("mz", mz_inv);
#endif
	return ;
}

void
camera_rot_xyz(struct camera *cam)
{
	matrix_t mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	camera_matrix(cam, &mt_inv, &mx_inv, &my_inv, &mz_inv);
	matrix_mul(&mx_inv, &my_inv, &mtmp);
	matrix_mul(&mtmp, &mz_inv, &mrot);
	matrix_mul(&mt_inv, &mrot, &cam->V);
}

void
camera_rot_zyx(struct camera *cam)
{
	matrix_t mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	camera_matrix(cam, &mt_inv, &mx_inv, &my_inv, &mz_inv);
	matrix_mul(&mz_inv, &my_inv, &mtmp);
	matrix_mul(&mtmp, &mx_inv, &mrot);
	matrix_mul(&mt_inv, &mrot, &cam->V);
}

void
camera_backface(struct camera *cam, struct object *obj)
{
	int i;
	if (obj->state & OBJECT4D_STATE_CULLED)
		return ;
	struct tri **rlist = &obj->rlist;
	for (i = 0; i < obj->tri_num; i++) {
		int v0;
		float dp;
		vector3_t n;
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		n.x = p->normal.x;
		n.y = p->normal.y;
		n.z = p->normal.z;
		dp = vector3_dot(&n, &cam->dir);
		if (dp >= 0.0f) {
			p->next = *rlist;
			*rlist = p;
		}
	}
	return ;
}



int
camera_transform(struct camera *cam)
{
	float n = cam->near_clip_z;
	float f = cam->far_clip_z;
	float as = cam->aspect_ratio;
	float theta = DEG_TO_RAD(cam->fov/2);
	float scale = 1 / tanf(theta);
	cam->P = (matrix_t) {
		scale/as,	0,		0,		0,
		0,		scale,		0,		0,
		0,		0,		(f+n)/(f-n),	1,
		0,		0,		2*f*n/(n-f),	0,
	};
	camera_rot_zyx(cam);
	return 0;
}

void
camera_viewport(struct camera *cam, struct object *obj)
{
	int i;
	float alpha = 0.5f * cam->viewport_width - 0.5f;
	float beta = 0.5f * cam->viewport_height - 0.5f;
	for (i = 0; i < obj->vertices_num; i++) {
		float w, x, y;
		vector4_t *v;
		v = &obj->vlist[i].v2f.sv_position;
		w = v->w;
		x = v->x * w;
		y = v->y * w;
		v->x = (x + 1.0f) * alpha;
		v->y = -y * beta + beta;
	}
}

