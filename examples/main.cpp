#include <unistd.h>
#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene_loader.h"
#include "pathtracer.h"
#include "raytracer.h"

int main(int argc, const char *argv[])
{
	int key = 0;
	if (argc < 2) {
		fprintf(stderr, "USAGE:%s scene\n", argv[0]);
		exit(0);
	}
	std::unique_ptr<irender> R;
	scene_loader sl;
	sl.load(argv[1]);

	auto &conf_screen = sl.getscreen();
	screen scrn(conf_screen.width, conf_screen.height);

	scene scene1;
	scene1.add(sl.getprimitives());

	auto &conf_cam= sl.getcamera();
	camera cam(conf_cam.eye, conf_cam.fov, conf_cam.znear, conf_cam.zfar);
	cam.yaw(conf_cam.yaw);

	auto &bg = sl.getbackground();

	switch (sl.getrender()) {
	case scene_loader::RASTERIZE:
		assert(0);
		break;
	case scene_loader::RAYTRACING:
		R.reset(new raytracer(cam, raytracer::RAYTRACING, bg));
		break;
	case scene_loader::PATHTRACING:
		R.reset(new raytracer(cam, raytracer::PATHTRACING, bg));
		break;
	default:
		assert(0);
	}
	while(key != 27)
	{
		scrn.clear();
		R->render(scene1, scrn, 64);
		scrn.show();
		key = cv::waitKey(10);
		std::cout << "key:" << key << std::endl;
		switch (key) {
		case 'w':
			std::cout << "w" << std::endl;
			cam.move(1.f);
			break;
		case 's':
			std::cout << "s" << std::endl;
			cam.move(-1.f);
			break;
		case 'a':
			std::cout << "a" << std::endl;
			cam.yaw(1.f);
			break;
		case 'd':
			std::cout << "d" << std::endl;
			cam.yaw(-1.f);
			break;
		}
	}
	return 0;
}

