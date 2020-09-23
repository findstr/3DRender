#include <unistd.h>
#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "raytracer.h"

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

