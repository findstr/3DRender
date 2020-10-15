#pragma once
#include "type.h"
#include "ray.h"
#include "screen.h"

namespace render {

class camera : public gpu {
public:
	CPU camera(vector3f eye, float fov, float znear, float zfar);
	CPU void jmp(vector3f eye);
	CPU void move(float step);
	CPU void pitch(float angle);
	CPU void yaw(float angle);
	CPU void roll(float angle);
public:
	GPU matrix4f view() const;
	GPU matrix4f projection(float aspect) const;
	GPU ray lookat(float aspect, float x, float y) const;
private:
	vector3f eye;
	float fov;
	float znear;
	float zfar;
	vector3f forward;
	matrix3f mrotation;
};


GPU ray
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


CPU 
camera::camera(vector3f eye, float fov, float znear, float zfar)
{
	this->eye= eye;
	this->fov = fov;
	this->znear = znear;
	this->zfar = zfar;
	this->forward = vector3f(0, 0, -1.f);
	this->mrotation = matrix3f::Identity();
}

GPU matrix4f
camera::view() const
{
	matrix4f view;
	matrix4f translate;
	view.assign(mrotation);
	view(3,3) = 1.f;
	translate.assign(
		1,0,0,-eye[0],
		0,1,0,-eye[1],
		0,0,1,-eye[2],
		0,0,0,1);
	return view*translate;
}

GPU matrix4f
camera::projection(float aspect) const
{
	float n = -znear;
	float f = -zfar;
	float t = std::tan(fov* 0.5f * 3.14f / 180.f) * std::abs(znear);
	float r = t * aspect;
	float b = -t;
	float l = -r;
	matrix4f persp_to_ortho, move_to_origin, ortho_to_ndc;
	persp_to_ortho.assign(
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n+f, -n*f,
		0, 0, 1, 0);
	move_to_origin.assign(
		1, 0, 0, -(l+r)/2,
		0, 1, 0, -(t+b)/2,
		0, 0, 1, -(n+f)/2,
		0, 0, 0, 1);
	ortho_to_ndc.assign(
		2/(r-l), 0, 0, 0,
		0, 2/(t-b), 0, 0,
		0, 0, 2/(n-f), 0,
		0, 0, 0, 1);
	return ortho_to_ndc * move_to_origin * persp_to_ortho;
}


CPU void
camera::jmp(vector3f eye)
{
	this->eye = eye;
}

CPU void
camera::move(float step)
{
	eye = eye + (mrotation * forward) * step;
	std::cout << "eye:" << eye << std::endl;
}

CPU void
camera::pitch(float angle) //x
{
	matrix3f m;
	float rad = deg2rad(-angle);
	float c = cos(rad);
	float s = sin(rad);
	m.assign(
		1, 0,  0,
		0, c, -s,
		0, s,  c);
	mrotation = m * mrotation;
}

CPU void
camera::yaw(float angle) //y
{
	matrix3f m;
	float rad = deg2rad(-angle);
	float c = cos(rad);
	float s = sin(rad);
	m.assign(
		c, 0,  s,
		0, 1,  0,
		-s,0,  c);
	mrotation = m * mrotation;
}

CPU void
camera::roll(float angle)//-z
{
	matrix3f m;
	float rad = deg2rad(-angle*forward.z());
	float c = cos(rad);
	float s = sin(rad);
	m.assign(
		c, -s, 0,
		s, c,  0,
		0, 0,  1);
	mrotation = m * mrotation;
}

}

