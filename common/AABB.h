#pragma once
#include <iostream>
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
	vector2f centroid() const {
		return 0.5f * (min + max);
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
	vector3f centroid() const {
		return 0.5f * (min + max);
	}
	void extend(const vector3f &v) {
		for (int i = 0; i < 3; i++) {
			min[i] = std::min(min[i], v[i]);
			max[i] = std::max(max[i], v[i]);
		}
	}
	void extend(const AABB3f &x) {
		for (int i = 0; i < 3; i++) {
			min[i] = std::min(min[i], x.min[i]);
			max[i] = std::max(max[i], x.max[i]);
		}
	}
	bool intersect(const ray &r) const {
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
		t_min = std::max(std::max(rmin[0], rmin[1]), rmin[2]);
		t_max = std::min(std::min(rmax[0], rmax[1]), rmax[2]);
		return t_min <= t_max && t_max >= 0;
	}
};

