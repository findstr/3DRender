#include <math.h>
#include <assert.h>
#include "mathlib.h"
#include "driver.h"
#include "primitive.h"
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

	tan_fov_div2 = 1.0f / tanf(DEG_TO_RAD(fov / 2));
	cam->view_dist = 0.5f * cam->viewplane_width * tan_fov_div2;
	assert(FCMP(cam->view_dist, 1.0f));
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
	struct tri **rlist = &obj->rlist;
	for (i = 0; i < obj->tri_num; i++) {
		int v0;
		float dp;
		vector4_t view;
		struct tri *p = &obj->plist[i];
		v0 = p->vert[0];
		vector4_sub(&cam->pos, &obj->vlist_local[v0].app.position, &view);
		dp = vector4_dot(&p->normal, &view);
		if (dp > 0.0f) {
			p->next = *rlist;
			*rlist = p;
		}
	}
	return ;
}

int
camera_transform(struct camera *cam)
{
	camera_rot_zyx(cam);
	return 0;
}

void
camera_perspective(struct camera *cam, struct object *obj)
{
	int i;
	for (i = 0; i < obj->vertices_num; i++) {
		vector4_t *v = &obj->vlist_local[i].v2f.sv_position;
		vector2_t *t = &obj->vlist_local[i].v2f.texcoord0;
		float z = 1 / v->z;
		v->x *= z;
		v->y *= z * cam->aspect_ratio;
		v->z = z;
		t->x *= z;
		t->y *= z;
	}
}

void
camera_viewport(struct camera *cam, struct object *obj)
{
	int i;
	float alpha = 0.5f * cam->viewport_width - 0.5f;
	float beta = 0.5f * cam->viewport_height - 0.5f;
	for (i = 0; i < obj->vertices_num; i++) {
		vector4_t *v = &obj->vlist_local[i].v2f.sv_position;
		v->x = (v->x + 1.0f) * alpha;
		v->y = -v->y * beta + beta;
	}
}

