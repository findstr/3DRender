#include "OBJ_Loader.hpp"
#include "auxiliary.h"
#include "primitive.h"

mesh::mesh(const std::string &name, std::shared_ptr<struct material> &mt)
	:mat(mt)
{
	areatotal = 0.f;
	bounds = AABB3f();
	model_matrix = matrix4f::Identity();
	scale_matrix <<
		1.f, 0, 0, 0,
		0, 1.f, 0, 0,
		0, 0, 1.f, 0,
		0, 0, 0, 1;
	objl::Loader loader;
	loader.LoadFile(name);
	auto &mesh = loader.LoadedMeshes[0];
	vertices.resize(mesh.Vertices.size());
	triangles.resize(mesh.Vertices.size());
	for (size_t i = 0; i < mesh.Vertices.size(); i += 3) {
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
}

mesh::mesh(const std::vector<vector3f> &verts,
	const std::vector<vector2f> &uv,
	const std::vector<int> &tri,
	std::shared_ptr<struct material> &mt):mat(mt)
{
	vertices.resize(verts.size());
	for (size_t i = 0; i < verts.size(); i++) {
		vertices[i].position = verts[i];
		vertices[i].texcoord = uv[i];
		bounds.extend(verts[i]);
	}
	triangles = tri;
}


bool
mesh::intersect_tri(const ray &r, hit &h, int idx) const
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
	h.normal = normal.normalized();
	h.color = vector3f(0.8f, 0.8f, 0.8f);
	h.barycentric = vector3f(b0, b1, b2);
	h.texcoord = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
	return true;
}

bool
mesh::intersect(const ray &r, hit &h) const
{
	if (!bounds.intersect(r))
		return false;
//	printf("intersect ok\n");
	int size = triangles.size() / 3;
	for (int i = 0; i < size; i++) {
		if (intersect_tri(r, h, i)) {
			bounds.intersect(r);
			h.obj = this;
			return true;
		}
	}
	return false;
}


float
mesh::sample(hit &h) const
{
	float sum = 0.f;
	float p = randomf() * areatotal;
	for (size_t i = 0; i < triangles.size(); i += 3) {
		auto &v0 = vertices[triangles[i+0]].position;
		auto &v1 = vertices[triangles[i+1]].position;
		auto &v2 = vertices[triangles[i+2]].position;
		auto e1 = v1 - v0;
		auto e2 = v2 - v0;
		float a = e1.cross(e2).norm() * 0.5f;
		sum += a;
		if (sum >= p) {
			float x = std::sqrt(randomf());
			float y = randomf();
			h.obj = this;
			h.normal = e1.cross(e2).normalized();
			h.point = v0 * (1.f - x) + v1 * (x * (1.f - y)) + v2 *  (x * y);
			return 1.f / areatotal;
		}
	}
	assert(!"never come here");
	return 0.f;
}

void
mesh::scale(const vector3f &s)
{
	scale_matrix <<
		s.x(), 0, 0, 0,
		0, s.y(), 0, 0,
		0, 0, s.z(), 0,
		0, 0, 0, 1;
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
	Eigen::Matrix4f translate;
	translate << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;

	model_matrix = translate * rotation * scale_matrix;
}

void
mesh::fetch(std::vector<triangle> &tri) const
{
	tri.resize(triangles.size()/3);
	for (size_t i = 0; i < triangles.size(); i += 3) {
		auto &t = tri[i/3];
		for (int j = 0; j < 3; j++) {
			auto &vert = vertices[triangles[i+j]];
			t.uv[j] = vert.texcoord;
			t.n[j] = vert.normal;
			t.ver[j] = tovector4f(vert.position, 1.f);
		}
	}
}


sphere::sphere(const vector3f &c, float r, std::shared_ptr<struct material> &m):
	center(c),
	radius(r),
	radius2(r * r),
	mat(m)
{}

bool
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

