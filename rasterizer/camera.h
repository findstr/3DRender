#pragma once
#include "type.h"
class camera {
public:
	camera(vector3f &eye, float fov, float aspect, float znear, float zfar);
	matrix4f view();
	matrix4f projection();
private:
	vector3f eye;
	float fov;
	float znear;
	float zfar;
	float aspect;
};

