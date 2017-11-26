#include <stdio.h>
#include "engine.h"
#include "camera.h"
#include "driver.h"
#include "plg.h"

#define WIDTH	(640)
#define HEIGHT	(480)
#define BITDEPTH (24)

vector4_t vscale = { 0.5f, 0.5f, 1.0f }, vpos = { 0, 0, 0, 1}, vrot = {0, 0, 0, 1};
struct camera cam;
struct object obj;

static void
update()
{
	int zi = driver_keydown('i', DRIVER_KEY_NORMAL);
	int zo = driver_keydown('o', DRIVER_KEY_NORMAL);
	vector4_t add = ZVECTOR4;
	if (zi != 0)
		add.z += 10.0f;
	if (zo != 0)
		add.z -= 10.0f;
	camera_move(&cam, &add);
	return ;
}

int main(int argc, char **argv)
{
	vector4_t cam_pos = { 0, 0, -50, 1 };
	vector4_t cam_dir = { 0, 10.0f, 0, 1 };
	mathlib_init();
	engine_start(WIDTH, HEIGHT, update);
	plg_load(&obj, "resource/cube1.plg", &IVECTOR4, &ZVECTOR4, &ZVECTOR4);

	camera_init(&cam, &cam_pos, &cam_dir, NULL,
		50.0f, 500.0f, 90.0f, WIDTH, HEIGHT);
	engine_add_camera(&cam);
	engine_add_object(&obj);
	engine_run();
	return 0;
}

