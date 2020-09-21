#pragma once
#include "type.h"
#include "ray.h"
#include "AABB.h"
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
};

class mesh {
public:
	mesh(const std::string &name, material *mt = nullptr);
	mesh(const std::vector<vector3f> &verts,
		const std::vector<vector2f> &uv,
		const std::vector<int> &tri,
		material *mt = nullptr);
	bool intersect(const ray &r, hit &h);
	void fetch(std::vector<triangle> &tri) const;
	void rot(float angle);
	void scale(const vector3f &s);
	matrix4f model() const;
	material *getmaterial() const;
private:
	bool intersect_tri(const ray &r, hit &h, int idx);
private:
	AABB3f bounds;
	material *mat;
	matrix4f model_matrix;
	matrix4f scale_matrix;
	std::vector<vector3f> normals;
	std::vector<int> triangles;
	std::vector<vertex> vertices;
};

