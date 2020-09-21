#include <unistd.h>
#include "screen.h"
#include "material.h"
#include "primitive.h"
#include "rasterizer.h"

float angle = 140.f;

int main()
{
	scene sce;
	screen scn(800, 800);
	//texture_shader shader;
	normal_shader shader;
	texture tex("models/spot/spot_texture.png");
	material m(&shader, &tex);

	std::vector<vector3f> verts = {{-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16}};
	std::vector<vector2f> uv = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
	std::vector<int> vertIndex = {0, 1, 3, 1, 2, 3};

	//mesh obj("models/spot/spot_triangulated_good.obj", &m);
	mesh obj(verts, uv, vertIndex, &m);
	obj.scale(vector3f(0.5, 0.5, 0.5));
	vector3f eye(0,0,10);
	camera cam(eye, 90.f, 1.f, 0.1f, 50);
	rasterizer r(&cam, &scn);
	sce.add(&obj);
	Eigen::Vector3f eye_pos = {0,0,50};
	while (true) {
		obj.rot(angle);
		scn.clear();
		r.render(sce);
		scn.set(100, 100, vector3f(1, 1, 1));
		scn.dump("a.ppm");
		/*
		scn.show();
		int key = cv::waitKey(10) & 0xfff;
		printf("key:%d angle:%f\n", key, angle);
		if (key == 'a')
			angle += 1.f;
			*/
		break;
	}
	return 0;
}

