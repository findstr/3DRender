#pragma once
#include "type.h"
#include "ray.h"
#include "screen.h"
class camera {
public:
	camera(vector3f eye, float fov, float znear, float zfar);
	void move(float step);
	void pitch(float angle);
	void yaw(float angle);
	void roll(float angle);
public:
	matrix4f view() const;
	matrix4f projection(float aspect) const;
	ray lookat(float aspect, float x, float y) const;
private:
	vector3f eye;
	float fov;
	float znear;
	float zfar;
	vector3f forward;
	matrix3f mrotation;
};

