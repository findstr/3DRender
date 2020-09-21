#pragma once
#include "type.h"
#include "ray.h"

struct AABB2f {
	vector2f min;
	vector2f max;
	AABB2f() {
		float inf = std::numeric_limits<float>::infinity();
		min = vector2f(inf, inf);
		max = vector2f(-inf, -inf);
	}
	template<typename T> void extend(const T &v) {
		for (int i = 0; i < 2; i++) {
			min[i] = std::min(min[i], v[i]);
			max[i] = std::max(max[i], v[i]);
		}
	}
};

struct AABB3f {
	vector3f min;
	vector3f max;
	AABB3f() {
		float inf = std::numeric_limits<float>::infinity();
		min = vector3f(inf, inf, inf);
		max = vector3f(-inf, -inf, -inf);
	}
	bool intersect(const ray &r) {
		float t_min, t_max;
		vector3f rmin, rmax;
		float inf = std::numeric_limits<float>::infinity();
		rmax = vector3f(inf, inf, inf);
		rmin = vector3f(-inf, -inf, -inf);
		auto &origin = r.origin;
		auto &dir = r.direction;
		for (int i = 0; i < 3; i++) {
			auto d = dir[i];
			if (d != 0) {
				rmin[i] = (min[i] - origin[i]) / d;
				rmax[i] = (max[i] - origin[i]) / d;
				if (d < 0)
					std::swap(rmin[i], rmax[i]);
			}
		}
		t_min = std::max(std::max(min[0], min[1]), min[2]);
		t_max = std::min(std::min(max[0], max[1]), max[2]);
		return t_min < t_max && t_max >= 0;
	}
	template<typename T> void extend(const T &v) {
		for (int i = 0; i < 3; i++) {
			min[i] = std::min(min[i], v[i]);
			max[i] = std::max(max[i], v[i]);
		}
	}
};

