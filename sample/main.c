#include <stdio.h>
#include "mathlib.h"
#include "pipeline.h"
#include "camera.h"
#include "driver.h"
#include "light.h"
#include "shader.h"
#include "bitmap.h"
#include "plane.h"
#include "plg.h"

#define WIDTH	(1024)
#define HEIGHT	(768)
#define BITDEPTH (24)

vector4_t vscale = { 0.5f, 0.5f, 1.0f }, vpos = { 0, 0, 0, 1}, vrot = {0, 0, 0, 1};
static struct camera cam;
static struct object obj, obj2;
static struct light *l;
static int frame = 0;

#if 0
static void _cross(matrix_t *a, matrix_t *b, matrix_t *c)
{
	matrix_mul(a, b, c);
}
#endif

static void _cross(quaternion_t *a, quaternion_t *b, quaternion_t *c)
{
	quaternion_cross(a, b, c);
}
static void
update()
{
	frame = frame + 1;
	quaternion_t rot;
	int zi = driver_keydown('i', DRIVER_KEY_NORMAL);
	int zo = driver_keydown('o', DRIVER_KEY_NORMAL);
	int rl = driver_keydown('a', DRIVER_KEY_NORMAL);
	int rr = driver_keydown('d', DRIVER_KEY_NORMAL);
	int rt = driver_keydown('w', DRIVER_KEY_NORMAL);
	int rb = driver_keydown('s', DRIVER_KEY_NORMAL);
	int zl = driver_keydown('e', DRIVER_KEY_NORMAL);
	int zr = driver_keydown('r', DRIVER_KEY_NORMAL);
	vector3_t add = ZVECTOR3;
	if (zi != 0) {
		l->ambient = rgba_add(l->ambient, RGBA(10, 10, 10, 0));
	}
	if (zo != 0) {
		l->ambient = rgba_sub(l->ambient, RGBA(10, 10, 10, 0));
	}
	//左手坐标系
	if (rl != 0) {
		obj.transform.pos.x -= 1.0f;
		//quaternion_rotate_y(&rot, 1.0f);
		//_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}
	if (rr != 0) {
		obj.transform.pos.x += 1.0f;
		//quaternion_rotate_y(&rot, -1.0f);
		//_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}
	if (rt != 0) {
		quaternion_rotate_x(&rot, 1.0f);
		_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}
	if (rb != 0) {
		quaternion_rotate_x(&rot, -1.0f);
		_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}
	if (zl != 0) {
		quaternion_rotate_z(&rot, 1.0f);
		_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}
	if (zr != 0) {
		quaternion_rotate_z(&rot, -1.0f);
		_cross(&obj.transform.rot, &rot, &obj.transform.rot);
	}

	camera_move(&cam, &add);
	return ;
}

int main(int argc, char **argv)
{
	quaternion_t rot = IQUATERNION;
	vector3_t cam_pos = { 0, 0, 0};
	vector3_t cam_dir = { 0, 0, 1.f};
	mathlib_init();
	pipeline_start(WIDTH, HEIGHT, update);
	vector3_t pos, scale;
#if 1
	vector3_init(&pos, 0, 0, 200.f);
	quaternion_rotate_x(&rot, 150.f);
	plane_load(&obj, &pos, &rot);

	vector3_init(&pos, 0, 0, 200.f);
	vector3_init(&scale, 5, 5, 5);
	plg_load(&obj, "resource/cube1.plg", &scale, &pos, &IQUATERNION);
#else
	plg_load(&obj, "resource/cube1.plg", &IVECTOR3, &ZVECTOR3, &pos);
	vector3_init(&pos, 0, 0, -1.0f);
	plg_load(&obj2, "resource/cube1.plg", &IVECTOR3, &ZVECTOR3, &pos);
#endif
	camera_init(&cam, &cam_pos, &cam_dir, NULL,
		0.3f, 1000.0f, 60.0f, WIDTH, HEIGHT);
	l = light_create();
	pipeline_add_camera(&cam);
	bitmap_load("resource/std.bmp", &obj.martial.texture);
	bitmap_load("resource/wall01.bmp", &obj2.martial.texture);
	obj.martial.shader.vert = shader_default_diffuse_vert;
	obj.martial.shader.frag = shader_default_diffuse_frag;
	obj2.martial.shader.vert = shader_default_diffuse_vert;
	obj2.martial.shader.frag = shader_default_diffuse_frag;
	pipeline_add_object(&obj);
	pipeline_add_object(&obj2);
	pipeline_run();
	return 0;
}

