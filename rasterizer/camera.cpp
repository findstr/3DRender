#include "camera.h"

camera::camera(vector3f &eye, float fov, float aspect, float znear, float zfar)
{
	this->eye= eye;
	this->fov = fov;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;
}

matrix4f
camera::view() {
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f translate;
	translate <<
		1,0,0,-eye[0],
		0,1,0,-eye[1],
		0,0,1,-eye[2],
		0,0,0,1;
	view = translate*view;
	return view;
}

matrix4f
camera::projection() {
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


