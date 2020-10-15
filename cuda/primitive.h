#pragma once
#include <memory>
#include "OBJ_Loader.hpp"
#include "type.h"
#include "ray.h"
#include "AABB.h"
#include "auxiliary.h"
#include "material.h"
#include "texture.h"

namespace render {

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

class primitive : public gpu {
public:
	enum type {
		MESH = 0,
		SPHERE = 1,
	};
	enum type type;
	primitive(enum type t):type(t){}
};

class mesh : public primitive {
public:
	CPU ~mesh() {}
	GPU bool intersect(const ray &r, hit &h) const ;
public:
	CPU	mesh(const std::string &name, struct material *mt);
	CPU void rot(float angle);
	CPU void scale(const vector3f &s);
	//GPU void fetch(gpu_vector<triangle> &tri) const;
	GPU float sample(thread_ctx *ctx, hit &h) const ;
	GPU float area() const  {return areatotal;};
	GPU matrix4f model() const {return model_matrix;};
	GPU vector3f position() const  {return vector3f(0,0,0);};
	ALL const struct material *material() const  { return mat;};
private:
	GPU bool intersect_tri(const ray &r, hit &h, int idx) const;
private:
	AABB3f bounds;
	float areatotal;
	struct material *mat;
	matrix4f model_matrix;
	matrix4f scale_matrix;
	gpu_vector<vector3f> normals;
	gpu_vector<int> triangles;
	gpu_vector<vertex> vertices;
};

class sphere : public primitive {
public:
	CPU sphere(const vector3f &c, float r, struct material *m);
	GPU bool intersect(const ray &r, hit &h) const ;
	GPU float sample(thread_ctx *ctx, hit &h) const  { assert(0); return 0.f; }
	GPU float area() const  {return 4.f*PI*radius2;}
	ALL const struct material *material() const  {return mat; };
	GPU vector3f position() const  {return center;};
private:
	vector3f center;
	float radius, radius2;
	struct material *mat;
};

GPU static inline float areaX(const primitive *ptr) {
	switch(ptr->type) {
	case primitive::MESH:
		return ((mesh *)(ptr))->area();
	case primitive::SPHERE:
		return ((sphere *)(ptr))->area();
	default:
		return 0.f;
	}
}

GPU static inline float sampleX(const primitive *ptr, thread_ctx *ctx, hit &h) {
	switch(ptr->type) {
	case primitive::MESH:
		return ((mesh *)(ptr))->sample(ctx, h);
	case primitive::SPHERE:
		return ((sphere *)(ptr))->sample(ctx, h);
	default:
		return 0.f;
	}
}

GPU static inline vector3f positionX(const primitive *ptr) {
	switch(ptr->type) {
	case primitive::MESH:
		return ((mesh *)(ptr))->position();
	case primitive::SPHERE:
		return ((sphere *)(ptr))->position();
	default:
		return vector3f(0.f,0.f,0.f);
	}
}

GPU static inline bool intersectX(const primitive *ptr, const ray &r, hit &h) {
	switch(ptr->type) {
	case primitive::MESH:
		return ((mesh *)(ptr))->intersect(r, h);
	case primitive::SPHERE:
		return ((sphere *)(ptr))->intersect(r, h);
	default:
		return false;
	}
}

ALL static inline const struct material *materialX(const primitive *ptr) {
	switch(ptr->type) {
	case primitive::MESH:
		return ((mesh *)(ptr))->material();
	case primitive::SPHERE:
		return ((sphere *)(ptr))->material();
	default:
		return nullptr;
	}
}


GPU bool
mesh::intersect_tri(const ray &r, hit &h, int idx) const
{
	vector3f v[3];
	vector2f uv[3];
	for (int i = 0; i < 3; i++) {
		const vertex &ver = vertices[triangles[i + idx * 3]];
		v[i] = ver.position;
		uv[i] = ver.texcoord;
	}
	auto E1 = v[1] - v[0];
	auto E2 = v[2] - v[0];
	auto normal = E1.cross(E2);
	if (r.direction.dot(normal) > 0.f)
		return false;
	auto S = r.origin - v[0];
	auto S1 = r.direction.cross(E2);
	auto S2 = S.cross(E1);
	auto t	= 1.f / S1.dot(E1) * S2.dot(E2);
	auto b1 = 1.f / S1.dot(E1) * S1.dot(S);
	auto b2 = 1.f / S1.dot(E1) * S2.dot(r.direction);
	auto b0 = 1 - b1 - b2;
	bool inside = (t >= 0.f &&
			b1 >= 0.f && b2 >= 0.f && b0 >= 0.f &&
			b1 <= 1.f && b2 <= 1.f && b0 <= 1.f);
	if (!inside)
		return false;
	h.triangleidx = idx;
	h.distance = t;
	h.point = r.point(t);
	h.normal = normal.normalized();
	h.color = vector3f(0.8f, 0.8f, 0.8f);
	h.barycentric = vector3f(b0, b1, b2);
	h.texcoord = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
	return true;
}

GPU bool
mesh::intersect(const ray &r, hit &h) const
{
	if (!bounds.intersect(r))
		return false;
	int size = (int)triangles.size() / 3;
	for (int i = 0; i < size; i++) {
		if (intersect_tri(r, h, i)) {
			h.obj = this;
			return true;
		}
	}
	return false;
}

GPU bool
sphere::intersect(const ray &r, hit &h) const
{
        // analytic solution
	vector3f L = r.origin - center;
	auto &dir = r.direction;
	float a = dir.dot(dir);
	float b = 2 * dir.dot(L);
	float c = L.dot(L) - radius2;
	float t0, t1;
	if (!solve_quadratic(a, b, c, t0, t1))
		return false;
	if (t0 < 0)
		t0 = t1;
	if (t0 < 0)
		return false;
	h.obj = this;
	h.distance = t0;
	h.point = r.point(t0);
	h.normal = (h.point - center).normalized();
	h.color = vector3f(0.8f, 0.8f, 0.8f);
	return true;
}


GPU float
mesh::sample(thread_ctx *ctx, hit &h) const
{
	float sum = 0.f;
	float p = ctx->rand() * areatotal;
	for (size_t i = 0; i < triangles.size(); i += 3) {
		const vertex &x0 = vertices[triangles[i+0]];
		const vertex &x1 = vertices[triangles[i+1]];
		const vertex &x2 = vertices[triangles[i+2]];
		auto &v0 = x0.position;
		auto &v1 = x1.position;
		auto &v2 = x2.position;
		auto e1 = v1 - v0;
		auto e2 = v2 - v0;
		float a = e1.cross(e2).norm() * 0.5f;
		sum += a;
		if (sum >= p) {
			float x = std::sqrt(ctx->rand());
			float y = ctx->rand();
			h.obj = this;
			h.normal = e1.cross(e2).normalized();
			h.point = v0 * (1.f - x) + v1 * (x * (1.f - y)) + v2 *  (x * y);
			return 1.f / areatotal;
		}
	}
	assert(!"never come here");
	return 0.f;
}
 

CPU mesh::mesh(const std::string &name, struct material *mt)
	:primitive(primitive::MESH), mat(mt)
{
	cpu_vector<vector3f> normals;
	cpu_vector<int> triangles;
	cpu_vector<vertex> vertices;
	areatotal = 0.f;
	bounds = AABB3f();
	model_matrix = matrix4f::Identity();
	scale_matrix.assign(
		1.f, 0, 0, 0,
		0, 1.f, 0, 0,
		0, 0, 1.f, 0,
		0, 0, 0, 1);
	objl::Loader loader;
	loader.LoadFile(name);
	auto &mesh = loader.LoadedMeshes[0];
	vertices.resize(mesh.Vertices.size());
	triangles.resize(mesh.Vertices.size());
	for (int i = 0; i < (int)mesh.Vertices.size(); i += 3) {
		auto *v = &vertices[i];
		auto *t = &triangles[i];
		for (int j = 0; j < 3; j++) {
			auto &dp = mesh.Vertices[i+j].Position;
			auto &dn = mesh.Vertices[i+j].Normal;
			auto &dt = mesh.Vertices[i+j].TextureCoordinate;
			v[j].position = vector3f(dp.X, dp.Y, dp.Z);
			v[j].normal = vector3f(dn.X, dn.Y, dn.Z);
			v[j].texcoord = vector2f(dt.X, dt.Y);
			bounds.extend(v[j].position);
			t[j] = i+j;
		}
		auto e1 = v[1].position - v[0].position;
		auto e2 = v[2].position - v[0].position;
		areatotal += e1.cross(e2).norm() * 0.5f;
	}
	this->normals = normals;
	this->triangles = triangles;
	this->vertices = vertices;
}


CPU void
mesh::scale(const vector3f &s)
{
	scale_matrix.assign(
		s.x(), 0, 0, 0,
		0, s.y(), 0, 0,
		0, 0, s.z(), 0,
		0, 0, 0, 1);
}

CPU void
mesh::rot(float angle)
{
	matrix4f rotation;
	angle = deg2rad(angle);
	rotation.assign(
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1);
	matrix4f translate;
	translate.assign(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

	model_matrix = translate * rotation * scale_matrix;
}

#if 0
GPU void
mesh::fetch(gpu_vector<triangle> &tri) const
{
	tri.resize(triangles.size()/3);
	for (size_t i = 0; i < triangles.size(); i += 3) {
		triangle t;
		for (int j = 0; j < 3; j++) {
			const vertex &vert = vertices[triangles[i+j]];
			t.uv[j] = vert.texcoord;
			t.n[j] = vert.normal;
			t.ver[j] = tovector4f(vert.position, 1.f);
		}
		tri[i/3] = t;
	}
}
#endif

CPU 
sphere::sphere(const vector3f &c, float r, struct material *m):
	primitive(primitive::SPHERE),
	center(c),
	radius(r),
	radius2(r * r),
	mat(m)
{}







}

