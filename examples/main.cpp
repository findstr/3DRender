#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene_loader.h"
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

	if (argc == 2) {
		int spp = 0;
		scrn.clear();
		while(key != 27)
		{
			R->render(scene1, scrn);
			++spp;
			scrn.scale(1.f / spp);
			scrn.show();
			key = cv::waitKey(10);
			std::cout << "key:" << key << std::endl << std::endl;
			switch (key) {
			case 'w':
				spp = 0;
				scrn.clear();
				std::cout << "w" << std::endl << std::endl;
				cam.move(10.f);
				break;
			case 's':
				spp = 0;
				scrn.clear();
				std::cout << "s" << std::endl << std::endl;
				cam.move(-10.f);
				break;
			case 'a':
				spp = 0;
				scrn.clear();
				std::cout << "a" << std::endl << std::endl;
				cam.yaw(1.f);
				break;
			case 'd':
				spp = 0;
				scrn.clear();
				std::cout << "d" << std::endl << std::endl;
				cam.yaw(-1.f);
				break;
			}
		}
	} else {
			int spp = sl.getspp();
			scrn.scale(1.f / spp);
			for (int i = 0; i < spp; i++) {
				std::cout << (float)i / spp << "\r";
				std::cout.flush();
				R->render(scene1, scrn);
			}
			scrn.dump("out.ppm");
	}
	return 0;
}

