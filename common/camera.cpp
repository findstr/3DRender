#include "auxiliary.h"
#include "camera.h"

camera::camera(vector3f eye, float fov, float znear, float zfar)
{
	this->eye= eye;
	this->fov = fov;
	this->znear = znear;
	this->zfar = zfar;
	this->forward = vector3f(0, 0, -1.f);
	this->mrotation = Eigen::Matrix3f::Identity();
}

matrix4f
camera::view() const
{
	matrix4f view;
	matrix4f translate;
	view <<
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 1;
	view.block(0,0, 3,3) = mrotation;
	translate <<
		1,0,0,-eye[0],
		0,1,0,-eye[1],
		0,0,1,-eye[2],
		0,0,0,1;
	return view*translate;
}

matrix4f
camera::projection(float aspect) const
{
	float n = -znear;
	float f = -zfar;
	float t = std::tan(fov* 0.5f * 3.14f / 180.f) * std::abs(znear);
	float r = t * aspect;
	float b = -t;
	float l = -r;
	Eigen::Matrix4f persp_to_ortho, move_to_origin, ortho_to_ndc;
	persp_to_ortho <<
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n+f, -n*f,
		0, 0, 1, 0;
	move_to_origin <<
		1, 0, 0, -(l+r)/2,
		0, 1, 0, -(t+b)/2,
		0, 0, 1, -(n+f)/2,
		0, 0, 0, 1;
	ortho_to_ndc <<
		2/(r-l), 0, 0, 0,
		0, 2/(t-b), 0, 0,
		0, 0, 2/(n-f), 0,
		0, 0, 0, 1;
	return ortho_to_ndc * move_to_origin * persp_to_ortho;
}

ray
camera::lookat(float aspect, float x, float y) const
{
	float scale = std::tan(deg2rad(fov * 0.5f));
	float t = znear * scale;
	float r = t * aspect;
	float xxx = -r + 2.f * r * x; //l + (r-l) * x
	float yyy = -t + 2.f * t * y; //b + (t-b) * y;
	float zzz = -znear;
	vector3f dir = (mrotation * vector3f(xxx, yyy, zzz)).normalized();
	return ray(eye, dir);
}

void
camera::jmp(vector3f eye)
{
	this->eye = eye;
}

void
camera::move(float step)
{
	eye = eye + (mrotation * forward) * step;
	std::cout << "eye:" << eye << std::endl;
}

void
camera::pitch(float angle) //x
{
	matrix3f m;
	float rad = deg2rad(-angle);
	float c = cos(rad);
	float s = sin(rad);
	m <<
		1, 0,  0,
		0, c, -s,
		0, s,  c;
	mrotation = m * mrotation;
}

void
camera::yaw(float angle) //y
{
	matrix3f m;
	float rad = deg2rad(-angle);
	float c = cos(rad);
	float s = sin(rad);
	m <<
		c, 0,  s,
		0, 1,  0,
		-s,0,  c;
	mrotation = m * mrotation;
}

void
camera::roll(float angle)//-z
{
	matrix3f m;
	float rad = deg2rad(-angle*forward.z());
	float c = cos(rad);
	float s = sin(rad);
	m <<
		c, -s, 0,
		s, c,  0,
		0, 0,  1;
	mrotation = m * mrotation;
}


