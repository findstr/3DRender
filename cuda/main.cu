#include <ctime>
#include "type.h"
#include "auxiliary.h"
#include "screen.h"
#include "scene_loader.h"
#include "raytracer.h"

namespace render {

#ifdef ENABLE_GPU

static ENTRY void init_(curandState *rand_state, int width, int height)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	int idx = y * width + x;
	if (idx >= width * height) return ;
	curand_init(1984, idx, 0, &rand_state[idx]);
}

static ENTRY void render(curandState *rand_state, raytracer *r, scene *sc, screen *scrn)
{
	thread_ctx ctx;
	raytracer R = *r;
	float width = scrn->getsize().x();
	float height = scrn->getsize().y();
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if (x >= width || y >= height) return; 
	int idx = y * width + x;
	if (idx >= width * height) return ;
	ctx.rnd = rand_state[idx];
	ctx.x = x;
	ctx.y = y;
	R.render(&ctx, *sc, *scrn);
}

#else

static random_ctx_t *init_(int width, int height) 
{

}

#endif

int main(int argc, const char *argv[])
{
#if 1
	int key = 0;
	if (argc < 2) {
		fprintf(stderr, "USAGE:%s scene\n", argv[0]);
		exit(0);
	}

	raytracer *R = nullptr;

	scene_loader sl;
	sl.load(argv[1]);

	auto &conf_screen = sl.getscreen();
	screen *scrn = new screen(conf_screen.width, conf_screen.height);

	std::cout << "scene:" << sizeof(scene) << std::endl;
	scene *scene1 = new scene();
	scene1->add(sl.getprimitives());

	auto &conf_cam= sl.getcamera();
	camera *cam = new camera(conf_cam.eye, conf_cam.fov, conf_cam.znear, conf_cam.zfar);
	cam->yaw(conf_cam.yaw);

	auto &bg = sl.getbackground();

	switch (sl.getrender()) {
	case scene_loader::RASTERIZE:
		assert(0);
		break;
	case scene_loader::RAYTRACING:
		R = new raytracer(*cam, raytracer::RAYTRACING, bg);
		break;
	case scene_loader::PATHTRACING:
		R = new raytracer(*cam, raytracer::PATHTRACING, bg);
		break;
	default:
		assert(0);
	}

	int nx = scrn->getsize().x();
	int ny = scrn->getsize().y();
	int tx = 8;
	int ty = 8;

	int width = nx;
	int height = ny;

	dim3 blocks((nx + tx - 1) / tx, (ny + ty - 1) / ty);
	dim3 threads(tx, ty);
	
	curandState *rand_state;
	checkX(cudaMalloc(&rand_state, sizeof(curandState) * width * height));

	GPU_CALL(blocks, threads, init_, rand_state, width, height);

	if (argc == 2) {
		int spp = 1;
		while(key != 27)
		{
			scrn->clear();
			scrn->scale(1.f / spp);
			clock_t start, stop;
			start = clock();
			GPU_CALL(blocks, threads, render, rand_state, R, scene1, scrn);
			stop = clock();
			std::cout << "takes:" << (double)(stop - start) / CLOCKS_PER_SEC << "s" << std::endl;
			scrn->show();
			key = cv::waitKey(1);
			std::cout << "key:" << key << std::endl << std::endl;
			switch (key) {
			case 'w':
				spp = 1;
				scrn->clear();
				std::cout << "w" << std::endl << std::endl;
				cam->move(10.f);
				break;
			case 's':
				spp = 1;
				scrn->clear();
				std::cout << "s" << std::endl << std::endl;
				cam->move(-10.f);
				break;
			case 'a':
				spp = 1;
				scrn->clear();
				std::cout << "a" << std::endl << std::endl;
				cam->yaw(1.f);
				break;
			case 'd':
				spp = 1;
				scrn->clear();
				std::cout << "d" << std::endl << std::endl;
				cam->yaw(-1.f);
				break;
			}
		}
	} else {
		/*
		int spp = sl.getspp();
			while (!R->render(scene1, scrn, spp));
			scrn.dump("out.ppm");
		*/
	}

	delete R;
	delete scrn;
	delete scene1;
	delete cam;
#endif
	return 0;
}

}


int main(int argc, const char *argv[]) 
{
	return render::main(argc, argv);
}

