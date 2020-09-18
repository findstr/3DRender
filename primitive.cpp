#include "OBJ_Loader.hpp"
#include "primitive.h"

mesh::mesh(const std::string &name, material *mt):mat(mt)
{
	model_matrix = matrix4f::Identity();
	objl::Loader loader;
	loader.LoadFile(name);
	auto &mesh = loader.LoadedMeshes[0];
	vertices.resize(mesh.Vertices.size());
	for (int i = 0; i < mesh.Vertices.size(); i += 3) {
		auto *v = &vertices[i];
		for (int j = 0; j < 3; j++) {
			auto &dp = mesh.Vertices[i+j].Position;
			auto &dn = mesh.Vertices[i+j].Normal;
			auto &dt = mesh.Vertices[i+j].TextureCoordinate;
			v[j].position = vector3f(dp.X, dp.Y, dp.Z);
			v[j].normal = vector3f(dn.X, dn.Y, dn.Z);
			v[j].texcoord = vector2f(dt.X, dt.Y);
		}
	}
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

