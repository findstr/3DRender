#pragma once
#include <memory>
#include "type.h"
#include "ray.h"
#include "AABB.h"
#include "BVH.h"
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
	virtual const std::string &name() const = 0;
	virtual float area() const = 0;
	virtual float uniform_sample(hit &h) const = 0;
	virtual vector3f position() const = 0;
	virtual bool intersect(const ray &r, hit &h) const = 0;
	virtual const struct material *material() const = 0;
};

class mesh : public primitive {
public:
	bool intersect(const ray &r, hit &h) const override;
public:
	mesh(const std::string &name,
		const vector3f &pos,
		float scale,
		std::shared_ptr<struct material> &mt);
	void fetch(std::vector<triangle> &tri) const;
	void rot(float angle);
	void scale(const vector3f &s);
	float uniform_sample(hit &h) const override;
	float area() const override {return areatotal;};
	const std::string &name() const override { return name_; }
	matrix4f model() const {return model_matrix;};
	vector3f position() const override {return vector3f(0,0,0);};
	const struct material *material() const override { return mat.get();};
private:
	bool intersect_tri(const ray &r, hit &h, int idx) const;
private:
	AABB3f bounds;
	float areatotal;
	std::string name_;
	matrix4f model_matrix;
	matrix4f scale_matrix;
	std::vector<vector3f> normals;
	std::vector<int> triangles;
	std::vector<vertex> vertices;
	std::shared_ptr<struct material> mat;
	std::unique_ptr<class BVH> bvh;
};

class sphere : public primitive {
public:
	sphere(const vector3f &c, float r, std::shared_ptr<struct material> &m);
	bool intersect(const ray &r, hit &h) const override;
	float uniform_sample(hit &h) const override {assert(0); return 0.f;}
	float area() const override {return 4.f*PI*radius2;}
	const std::string &name()const override { return name_; }
	const struct material *material() const override {return mat.get(); };
	vector3f position() const override {return center;};
public:
	vector3f center;
	std::string name_;
	float radius, radius2;
	std::shared_ptr<struct material> mat;
};

