#ifndef _CAMERA_H
#define _CAMERA_H

#include "mathlib.h"
#include "primitive.h"

#define CULL_X_PLANE	(1 << 0)
#define CULL_Y_PLANE	(1 << 1)
#define CULL_Z_PLANE	(1 << 2)
#define CULL_XYZ_PLANE	(CULL_X_PLANE | CULL_Y_PLANE | CULL_Z_PLANE)

struct camera {
	int state;
	vector4_t pos;
	vector4_t dir;

	vector4_t u;
	vector4_t v;
	vector4_t n;
	vector4_t target;

	float view_dist;
	float fov;

	float near_clip_z;
	float far_clip_z;

	plane3d_t rt_clip_plane;
	plane3d_t lt_clip_plane;
	plane3d_t tp_clip_plane;
	plane3d_t bt_clip_plane;

	float viewplane_width;
	float viewplane_height;

	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;

	float aspect_ratio;

	matrix_t mcam;
	matrix_t mper;
	matrix_t mscr;
	struct camera *next;
};

void camera_init(struct camera *cam, vector4_t *pos,
		vector4_t *dir, vector4_t *target,
		float near_clip_z, float far_clip_z, float fov,
		float viewport_width, float viewport_height);
void camera_move(struct camera *cam, const vector4_t *add);
void camera_rot_xyz(struct camera *cam);
void camera_rot_zyx(struct camera *cam);
int camera_transform(struct camera *cam, struct object *obj);
/*
void camera_cull(struct camera *cam, struct object4d *obj, int cull_flag);
void camera_backface(struct camera *cam, struct object4d *obj);
void camera_perspective(struct camera *cam, struct object4d *obj);
void camera_viewport(struct camera *cam, struct object4d *obj);
void camera_draw(struct camera *cam, struct object4d *obj);
*/

#endif

