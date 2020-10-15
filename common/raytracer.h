#pragma once
#include "type.h"
#include "screen.h"
#include "camera.h"
#include "scene.h"
#include "primitive.h"

class irender {
public:
	virtual ~irender() {}
	virtual void render(const scene &sc, screen &scrn) = 0;
};

class raytracer : public irender {
public:
	enum mode {
		RAYTRACING,
		PATHTRACING,
	};
public:
	raytracer(const camera &c, enum mode m = RAYTRACING, vector3f bg = vector3f(0,0,0));
	void setmode(enum mode m);
	void setbackground(vector3f c);
	void render(const scene &sc, screen &scrn) override;
private:
	vector3f trace(ray r, int depth);
	vector3f raytracing(const ray &r, const hit &h, int depth);
	vector3f pathtracing_r(const ray &r, const hit &h, int depth);
	vector3f pathtracing(const ray &r, const hit &h, int depth);
private: //material
	vector3f light(const ray &r, const hit &h, int depth);
	vector3f glass(const ray &r, const hit &h, int depth);
private:
	int spp_n = 0;
	enum mode mode_;
	const camera &camera_;
	const scene *sc = nullptr;
	vector3f background = vector3f(0, 0, 0);
};

