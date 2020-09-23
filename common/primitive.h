#pragma once
#include <memory>
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

struct light {
	light(vector3f p, vector3f i)
		:position(p), intensity(i) {}
	vector3f position;
	vector3f intensity;
};

class primitive {
public:
	virtual ~primitive() = default;
	virtual bool intersect(const ray &r, hit &h) const = 0;
	virtual material *getmaterial() const = 0;
};

class mesh : public primitive {
public:
	bool intersect(const ray &r, hit &h) const override;
public:
	mesh(const std::string &name, std::shared_ptr<material> &mt);
	mesh(const std::vector<vector3f> &verts,
		const std::vector<vector2f> &uv,
		const std::vector<int> &tri,
		std::shared_ptr<material> &mt);
	void fetch(std::vector<triangle> &tri) const;
	void rot(float angle);
	void scale(const vector3f &s);
	matrix4f model() const;
	material *getmaterial() const override;
private:
	bool intersect_tri(const ray &r, hit &h, int idx) const;
private:
	AABB3f bounds;
	std::shared_ptr<material> mat;
	matrix4f model_matrix;
	matrix4f scale_matrix;
	std::vector<vector3f> normals;
	std::vector<int> triangles;
	std::vector<vertex> vertices;
};

class sphere : public primitive {
public:
	sphere(const vector3f &c, float r, std::shared_ptr<material> &m);
	bool intersect(const ray &r, hit &h) const override;
	material *getmaterial() const override;
private:
	std::shared_ptr<material> mat;
	vector3f center;
	float radius, radius2;
};

