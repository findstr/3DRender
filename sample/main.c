#include <stdio.h>
#include "mathlib.h"
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
struct object4d obj;

static void
display(void)
{
	/*
	uint8_t *frame = graphic_lock_secondary();
	int i, j;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			uint8_t c = ((((i&0x8)==0)^(((j&0x8))==0)))*255;
			uint8_t *buf = frame + (i * WIDTH + j) * (BITDEPTH / 8);
			buf[0] = c;
			buf[1] = c;
			buf[2] = c;
		}
	}
	graphic_unlock_secondary(frame);
	graphic_flip_display();
	*/
	printf("=======================\n");
	//vector4_print("h1",&obj.vlist_local[0]);
	transform_model2world(&obj, TRANSFORM_LOCAL_TRANS);
	//vector4_print("h2",&obj.vlist_trans[0]);
	camera_rot_zyx(&cam);
	transform_obj(&obj, &cam.mcam, TRANSFORM_TRANS_ONLY, 0);
	//vector4_print("h3",&obj.vlist_trans[0]);
	camera_perspective(&cam, &obj);
	//vector4_print("h4",&obj.vlist_trans[0]);
	camera_viewport(&cam, &obj);
	//vector4_print("h5-1",&obj.vlist_trans[0]);
	//vector4_print("h5-2",&obj.vlist_trans[1]);
	//vector4_print("h5-3",&obj.vlist_trans[2]);
	camera_draw(&cam, &obj);
	graphic_flip_display();
}

int main(int argc, char **argv)
{
	mathlib_init();
	plg_load(&obj, "resource/cube1.plg", &IVECTOR4, &ZVECTOR4, &ZVECTOR4);
	camera_init(&cam, &cam_pos, &cam_dir, NULL, 50.0f, 500.0f, 90.0f, WIDTH, HEIGHT);
	graphic_run(WIDTH, HEIGHT, display);
	return 0;
}
