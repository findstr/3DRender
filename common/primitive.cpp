#include "OBJ_Loader.hpp"
#include "primitive.h"

mesh::mesh(const std::string &name, material *mt):mat(mt)
{
	bounds = AABB3f();
	model_matrix = matrix4f::Identity();
	objl::Loader loader;
	loader.LoadFile(name);
	auto &mesh = loader.LoadedMeshes[0];
	vertices.resize(mesh.Vertices.size());
	triangles.resize(mesh.Vertices.size());
	for (int i = 0; i < mesh.Vertices.size(); i += 3) {
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
	}
}

bool
mesh::intersect_tri(const ray &r, hit &h, int idx)
{
	vector3f v[3];
	vector2f uv[3];
	for (int i = 0; i < 3; i++) {
		auto &ver = vertices[triangles[i + idx * 3]];
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
	h.normal = normal;
	h.color = vector3f(0.8f, 0.8f, 0.8f);
	h.barycentric = vector3f(b0, b1, b2);
	h.texcoord = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
	return true;
}

bool
mesh::intersect(const ray &r, hit &h)
{
	if (!bounds.intersect(r))
		return false;
	int size = triangles.size() / 3;
	for (int i = 0; i < size; i++) {
		if (intersect_tri(r, h, i)) {
			h.obj = this;
			return true;
		}
	}
	return false;
}

void
mesh::rot(float angle)
{
	Eigen::Matrix4f rotation;
	angle = deg2rad(angle);
	rotation <<
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1;

	Eigen::Matrix4f scale;
	scale << 2.5, 0, 0, 0,
			  0, 2.5, 0, 0,
			  0, 0, 2.5, 0,
			  0, 0, 0, 1;

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;

	model_matrix = translate * rotation * scale;
}

material *
mesh::getmaterial() const
{
	return mat;
}

matrix4f
mesh::model() const
{
	return model_matrix;
}

void
mesh::fetch(std::vector<triangle> &tri) const
{
	tri.resize(vertices.size()/3);
	for (int i = 0; i < vertices.size(); i += 3) {
		auto &t = tri[i/3];
		for (int j = 0; j < 3; j++) {
			t.uv[j] = vertices[i+j].texcoord;
			t.n[j] = vertices[i+j].normal;
			t.ver[j] = tovector4f(vertices[i+j].position, 1.f);
		}
	}
}

