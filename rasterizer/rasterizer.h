#pragma once
#include "screen.h"
#include "scene.h"
#include "camera.h"
#include "primitive.h"

class rasterizer {
public:
	rasterizer(camera *c, screen *scn);
	void render(const scene &s);
private:
	void draw_line(const vector4f &begin, const vector4f &end);
	void render_triangle(const triangle &tri, const material *m, vector3f viewpos[3]);
	void render_mesh(const mesh &m);
private:
	screen *scn;
	camera *cam;
};

