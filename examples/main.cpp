#include <unistd.h>
#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene_loader.h"
#include "pathtracer.h"
#include "raytracer.h"

int main()
{
	int key = 0;
	scene scene1;
	screen scrn(800, 800);
	pathtracer pr;

	vector3f eye_pos = vector3f(0,0,10);
	camera cam(vector3f(0,0,10), 90.f, 1.f, 100.f);
	raytracer rr(cam);;
	scene_loader sl;
	sl.load(scene1, "scene/whitted.scene");
	while(key != 27)
	{
		scrn.clear();
		rr.render(scene1, scrn);
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

