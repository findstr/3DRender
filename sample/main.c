#include <stdio.h>
#include "engine.h"
#include "camera.h"
#include "graphic.h"
#include "transform.h"
#include "plg.h"

#define WIDTH	(640)
#define HEIGHT	(480)
#define BITDEPTH (24)

vector4_t cam_pos = { 0, 0, -100, 1 };
vector4_t cam_dir = { 0, 0, 0, 1 };
vector4_t vscale = { 0.5f, 0.5f, 1.0f }, vpos = { 0, 0, 0, 1}, vrot = {0, 0, 0, 1};
struct camera cam;
struct object obj;

int main(int argc, char **argv)
{
	mathlib_init();
	engine_start(WIDTH, HEIGHT);
	plg_load(&obj, "resource/cube1.plg", &IVECTOR4, &ZVECTOR4, &ZVECTOR4);
	camera_init(&cam, &cam_pos, &cam_dir, NULL,
		50.0f, 500.0f, 90.0f, WIDTH, HEIGHT);
	engine_add_camera(&cam);
	engine_add_object(&obj);
	engine_run();
	return 0;
}
