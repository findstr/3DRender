#include <unistd.h>
#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"

class tex_program : public itexture {
public:
	vector3f sample(const vector2f &uv) const override {
		float scale = 5;
		float pattern = (fmodf(uv.x() * scale, 1) > 0.5) ^ (fmodf(uv.y() * scale, 1) > 0.5);
		vector3f a(0.815, 0.235, 0.031);
		vector3f b(0.937, 0.937, 0.231);
		return lerp(pattern, a, b);
	}
};

class tex_program2 : public itexture {
public:
	tex_program2(vector3f diffuse) {
		this->diffuse = diffuse;
	}
	vector3f sample(const vector2f &uv) const override {
		return diffuse;
	}
private:
	vector3f diffuse;
};

#if 0
#include "raytracer.h"
static std::unique_ptr<primitive>
build_plan()
{
	//mesh
	std::vector<vector3f> verts = {{-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16}};
	std::vector<vector2f> uv = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
	std::vector<int> vertIndex = {0, 1, 3, 1, 2, 3};

	std::shared_ptr<itexture> tex(new tex_program());
	auto mat = std::make_shared<material>(tex);
	std::unique_ptr<primitive> obj(new mesh(verts, uv, vertIndex, mat));
	mat->type = material::GLOSSY;
	return std::move(obj);
}

static std::unique_ptr<primitive>
build_sphere1()
{
	std::shared_ptr<itexture> tex(new tex_program2(vector3f(0.95, 0.93, 0.88)));
	auto mat = std::make_shared<material>(tex);
	std::unique_ptr<primitive> obj(new sphere(vector3f(-1, 0, -12), 2, mat));
	mat->type = material::SPECULAR;
	mat->specularexponent = 2;
	mat->Kd = 0.1f;
	mat->Ks = 0.8f;
	return std::move(obj);
}

static std::unique_ptr<primitive>
build_sphere2()
{
	std::shared_ptr<itexture> tex(new tex_program());
	auto mat = std::make_shared<material>(tex);
	std::unique_ptr<primitive> obj(new sphere(vector3f(0.5, -0.5, -8), 2, mat));
	mat->type = material::GLASS;
	mat->ior = 5.f;
	return std::move(obj);
}

int main()
{
	scene scene1;
	screen scn(800, 800);

	auto plan = build_plan();
	auto sph1 = build_sphere1();
	auto sph2 = build_sphere2();
	scene1.add(plan);
	scene1.add(sph1);
	scene1.add(sph2);

	//light
	light l1(vector3f(-20, 70, 20), vector3f(0.5, 0.5, 0.5));
	light l2(vector3f(30, 50, -12), vector3f(0.5, 0.5, 0.5));
	scene1.add(&l1);
	scene1.add(&l2);

	raytracer r;

	scn.clear();
	r.render(scene1, scn);
	scn.set(100, 100, vector3f(1, 1, 1));
	scn.dump("a.ppm");
	return 0;
}
#else

#include "pathtracer.h"
int main()
{
	scene scene1;
	screen scrn(700, 700);
	vector3f emission =
		 8.0f * vector3f(0.747f+0.058f,0.747f+0.258f,0.747f) +
		15.6f * vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) +
		18.4f * vector3f(0.737f+0.642f,0.737f+0.159f,0.737f);
	std::shared_ptr<itexture> tex_red(new tex_program2(vector3f(0.63f, 0.065f, 0.05f)));
	std::shared_ptr<itexture> tex_green(new tex_program2(vector3f(0.14f, 0.45f, 0.091f)));
	std::shared_ptr<itexture> tex_white(new tex_program2(vector3f(0.725f, 0.71f, 0.68f)));
	std::shared_ptr<itexture> tex_white2(new tex_program2(vector3f(0.95f, 0.93f, 0.88f)));
	std::shared_ptr<material> mat_micro(new material(tex_white2, material::SPECULAR));
	std::shared_ptr<material> mat_red(new material(tex_red));
	std::shared_ptr<material> mat_green(new material(tex_green));
	std::shared_ptr<material> mat_white(new material(tex_white));
	std::shared_ptr<material> mat_light(new material(emission, material::LIGHT));

	struct object {
		std::string name;
		std::shared_ptr<material> mat;
	} objs[] = {
		{"models/cornellbox/floor.obj", mat_white},
		{"models/cornellbox/shortbox.obj", mat_white},
		{"models/cornellbox/tallbox.obj", mat_micro},
		{"models/cornellbox/left.obj", mat_red},
		{"models/cornellbox/right.obj", mat_green},
		{"models/cornellbox/light.obj", mat_light},
	};
	for (auto &iter:objs) {
		std::unique_ptr<primitive> obj(new mesh(iter.name, iter.mat));
		scene1.add(obj);
	}

	pathtracer r;

	scrn.clear();
	r.render(scene1, scrn);
	scrn.dump("out.ppm");

	return 0;
}

#endif


