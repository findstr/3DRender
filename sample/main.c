#include <stdio.h>
#include "mathlib.h"
#include "pipeline.h"
#include "camera.h"
#include "driver.h"
#include "light.h"
#include "shader.h"
#include "bitmap.h"
#include "plg.h"

#define WIDTH	(640)
#define HEIGHT	(480)
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
	vector4_t add = ZVECTOR4;
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
#if 1
	vector4_t cam_pos = { 0, 0, -20, 1 };
	vector4_t cam_dir = { 0, 10.0f, 0, 1 };
	mathlib_init();
	pipeline_start(WIDTH, HEIGHT, update);
	vector4_t pos;
	vector4_init(&pos, 0, 0, 0);
	plg_load(&obj, "resource/cube1.plg", &IVECTOR4, &ZVECTOR4, &pos);
	vector4_init(&pos, 0, 0, -1.0f);
	plg_load(&obj2, "resource/cube1.plg", &IVECTOR4, &ZVECTOR4, &pos);
	camera_init(&cam, &cam_pos, &cam_dir, NULL,
		50.0f, 500.0f, 90.0f, WIDTH, HEIGHT);
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
#else
	vector4_t a, b, c, u, v, n1, n2, n3, xRy, yRz, zRx;
	quaternion_t rx, ry, rz;
	vector4_init(&a, 0, 1, 0);
	vector4_init(&b, 1, 1, 0);
	vector4_init(&c, 1, 0, 0);

	vector4_sub(&b, &a, &u);
	vector4_sub(&c, &a, &v);
	vector4_cross(&u, &v, &n1);
	vector4_print("normal1", &n1);


	quaternion_rotate_x(&rx, 90.0f);
	vector4_mul_quaternion(&a, &rx, &a);
	vector4_mul_quaternion(&b, &rx, &b);
	vector4_mul_quaternion(&c, &rx, &c);

	vector4_sub(&b, &a, &u);
	vector4_sub(&c, &a, &v);
	vector4_cross(&u, &v, &n2);
	vector4_print("normal2", &n2);


	vector4_mul_quaternion(&n1, &rx, &n3);
	vector4_print("normal3", &n3);

	quaternion_print("rx", &rx);
	quaternion_print("ry", &ry);
	quaternion_print("rz", &rz);


#endif
	/*
	quaternion_t c1, c2, c3, c4;
	quaternion_t res;
	quaternion_rotate_x(&c1, 90.0f);
	quaternion_rotate_z(&c2, 90.0f);
	quaternion_inverse(&c1, &c3);
	quaternion_inverse(&c2, &c4);
	quaternion_t v = {0.0f, 0.0f, 0.0f, 1.0f};


	quaternion_cross(&c3, &v, &v);
	quaternion_cross(&v, &c1, &v);

	quaternion_print("res:", &v);
	v.y = -1;
	quaternion_cross(&c4, &v, &v);
	quaternion_cross(&v, &c2, &v);

	quaternion_print("res:", &v);

	quaternion_print("res:", &v);
	quaternion_print("c4:", &c4);
	quaternion_print("c4':", &c4);
	*/

	return 0;
}

