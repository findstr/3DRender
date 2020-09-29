#pragma once
#include "type.h"
#include "screen.h"
#include "camera.h"
#include "scene.h"
#include "primitive.h"

class raytracer {
public:
	raytracer (const camera &c);
	void render(const scene &sc, screen &scrn, int spp = 1);
private:
	vector3f trace(ray r, int depth);
private: //material
	vector3f specular(const ray &r, const hit &h, int depth);
	vector3f glass(const ray &r, const hit &h, int depth);
	vector3f glossy(const ray &r, const hit &h, int depth);
private:
	float fov = 90;
	const camera &camera_;
	const scene *sc = nullptr;
	vector3f eye_pos = vector3f(0,0,10);
	vector3f background = vector3f(0.235294, 0.67451, 0.843137);
};

