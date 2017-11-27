#include <math.h>
#include "mathlib.h"
#include "driver.h"
#include "transform.h"
#include "camera.h"

void
camera_init(struct camera *cam, vector4_t *pos,
		vector4_t *dir, vector4_t *target,
		float near_clip_z, float far_clip_z, float fov,
		float viewport_width, float viewport_height)
{
	vector3_t vn;
	float tan_fov_div2;
	cam->pos = *pos;
	cam->dir = *dir;
	vector4_init(&cam->u, 1.0f, 0.0f, 0.0f);
	vector4_init(&cam->v, 0, 1, 0);
	vector4_init(&cam->n, 0, 1, 0);
	if (target)
		cam->target = *target;
	else
		vector4_zero(&cam->target);
	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;

	cam->viewport_width = viewport_width;
	cam->viewport_height = viewport_height;

	cam->viewport_center_x = (viewport_width - 1) / 2;
	cam->viewport_center_y = (viewport_height - 1) / 2;

	cam->aspect_ratio = viewport_width / viewport_height;

	cam->mcam = IMATRIX;
	cam->mper = IMATRIX;
	cam->mscr = IMATRIX;

	cam->fov = fov;

	cam->viewplane_width = 2.0f;
	cam->viewplane_height = 2.0f / cam->aspect_ratio;

	tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));
	//FIXME?
	cam->view_dist = 0.5f * cam->viewplane_width * tan_fov_div2;
	if (fov == 90.0f) {
		vector3_init_normalize(&vn, 1.0f, 0.0f , -1.0f);//x,z
		plane3d_init(&cam->rt_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, -1.0f, 0.0f , -1.0f);//-x,z
		plane3d_init(&cam->lt_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, 0.0f, 1.0f, -1.0f);//y,z
		plane3d_init(&cam->tp_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, 0.0f, -1.0f, -1.0f);//-y,z
		plane3d_init(&cam->bt_clip_plane, &ZVECTOR3, &vn);
	} else {
		float z = cam->view_dist;
		float x = cam->viewplane_width / 2.0f;

		vector3_init_normalize(&vn, z, 0.0f, -x); //x,z
		plane3d_init(&cam->rt_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, -z, 0.0f, -x);//-x,z
		plane3d_init(&cam->lt_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, 0.0f, z, -x);//y,z
		plane3d_init(&cam->tp_clip_plane, &ZVECTOR3, &vn);

		vector3_init_normalize(&vn, 0.0f, -z, -x); //-y,z
		plane3d_init(&cam->bt_clip_plane, &ZVECTOR3, &vn);
	}
	return ;
}

void
camera_move(struct camera *cam, const vector4_t *add)
{
	vector4_add(&cam->pos, add, &cam->pos);
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
	float cos_theta, sin_theta;
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
	matrix_mul(&mt_inv, &mrot, &cam->mcam);
}

void
camera_rot_zyx(struct camera *cam)
{
	matrix_t mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	camera_matrix(cam, &mt_inv, &mx_inv, &my_inv, &mz_inv);
	matrix_mul(&mz_inv, &my_inv, &mtmp);
	matrix_mul(&mtmp, &mx_inv, &mrot);
	matrix_mul(&mt_inv, &mrot, &cam->mcam);
}

void
camera_cull(struct camera *cam, struct object *obj, int cull_flag)
{
	vector4_t sphere_pos;
	float r = obj->radius_max;
	float z = sphere_pos.z;
	vector4_mul_matrix(&obj->transform.pos, &cam->mcam, &sphere_pos);
	if (cull_flag & CULL_Z_PLANE) {
		if ((z - r) > cam->far_clip_z || (z + r) > cam->near_clip_z) {
			obj->state |= OBJECT4D_STATE_CULLED;
			return ;
		}
	}
	if (cull_flag & CULL_X_PLANE) {
		float x = sphere_pos.x;
		float z_test = 0.5f * cam->viewplane_width * z / cam->view_dist;
		if ((x - r) > z_test || (x + r) < -z_test) {
			obj->state |= OBJECT4D_STATE_CULLED;
			return ;
		}
	}
	if (cull_flag & CULL_Y_PLANE) {
		float y = sphere_pos.y;
		float z_test = 0.5f * cam->viewplane_height * z / cam->view_dist;
		if ((y - r) > z_test || (y + r) < -z_test) {
			obj->state |= OBJECT4D_STATE_CULLED;
			return ;
		}
	}
	return ;
}

void
camera_backface(struct camera *cam, struct object *obj)
{
	int i;
	if (obj->state & OBJECT4D_STATE_CULLED)
		return ;
	for (i = 0; i < obj->polys_num; i++) {
		float dp;
		int v0, v1, v2;
		vector4_t u,v,n;
		vector4_t view;
		struct poly4d *p = &obj->plist[i];
		if (!((p->state & POLY4D_STATE_ACTIVE) ||
			(p->state & POLY4D_STATE_CLIPPED) ||
			(p->attr & POLY4D_ATTR_2SIDED) ||
			(p->state & POLY4D_STATE_BACKFACE)))
			continue;
		v0 = p->vert[0];
		v1 = p->vert[1];
		v2 = p->vert[2];

		vector4_sub(&obj->vlist_trans[v1], &obj->vlist_trans[v0], &u);
		vector4_sub(&obj->vlist_trans[v2], &obj->vlist_trans[v0], &v);
		vector4_cross(&u, &v, &n);
		vector4_sub(&cam->pos, &obj->vlist_trans[v0], &view);
		dp = vector4_dot(&n, &view);
		if (dp <= 0.0f)
			p->state |= POLY4D_STATE_BACKFACE;
	}
	return ;
}

void
camera_perspective(struct camera *cam, struct object *obj)
{
	int i;
	for (i = 0; i < obj->vertices_num; i++) {
		float z = obj->vlist_trans[i].z;
		float scale = cam->view_dist / z;
		obj->vlist_trans[i].x = obj->vlist_trans[i].x * scale;
		obj->vlist_trans[i].y = obj->vlist_trans[i].y * scale * cam->aspect_ratio;
	}
}

void
camera_viewport(struct camera *cam, struct object *obj)
{
	int i;
	float alpha = 0.5f * cam->viewport_width - 0.5f;
	float beta = 0.5f * cam->viewport_height - 0.5f;
	for (i = 0; i < obj->vertices_num; i++) {
		obj->vlist_trans[i].x = alpha + alpha * obj->vlist_trans[i].x;
		obj->vlist_trans[i].y = beta - beta * obj->vlist_trans[i].y;
	}
}

void
camera_transform(struct camera *cam, struct object *obj)
{
	camera_rot_zyx(cam);
	transform_obj(obj, &cam->mcam, 0);
	camera_perspective(cam, obj);
	camera_viewport(cam, obj);
}

