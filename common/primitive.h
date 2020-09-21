#pragma once
#include "type.h"
#include "auxiliary.h"
#include "material.h"
#include "texture.h"

struct vertex {
	vector3f color;
	vector3f position;
	vector3f normal;
	vector2f texcoord;
};

struct triangle {
	vector2f uv[3];
	vector3f n[3];
	vector4f ver[3];
	vector3f normal;
};

class mesh {
public:
	mesh(const std::string &name, material *mt = nullptr);
	matrix4f model() const;
	void fetch(std::vector<triangle> &tri) const;
	void rot(float angle);
	material *getmaterial() const;
private:
	material *mat;
	matrix4f model_matrix;
	std::vector<vector3f> normals;
	std::vector<int> indices;
	std::vector<vertex> vertices;
};

