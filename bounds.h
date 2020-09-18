#pragma once
#include "type.h"

struct bounds2f {
	vector2f min;
	vector2f max;
	bounds2f();
	template<typename T> void extend(const T &v) {
		for (int i = 0; i < 2; i++) {
			min[i] = std::min(min[i], v[i]);
			max[i] = std::max(max[i], v[i]);
		}
	}
};

