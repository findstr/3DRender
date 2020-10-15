#pragma once
#include "type.h"

namespace render {

class primitive;

struct ray {
	GPU ray(vector3f o, const vector3f &d):origin(o) {
		direction = d;
	}
	GPU vector3f point(float t) const {
		return origin + t * direction;
	}
	GPU vector3f move(float t) const {
		return origin + t * direction;
	}
	vector3f origin;
	vector3f direction;
};

struct hit {
	GPU hit() {
		obj = nullptr;
		triangleidx = -1;
		distance = INFINITY;
		point = vector3f(0,0,0);
		normal = vector3f(0,0,0);
		color = vector3f(0,0,0);
		texcoord = vector2f(0,0);
		barycentric = vector3f(0,0,0);
	}
	const primitive *obj;
	int triangleidx;
	float distance;
	vector3f point;
	vector3f normal;
	vector3f color;
	vector2f texcoord;
	vector3f barycentric;

};
}
