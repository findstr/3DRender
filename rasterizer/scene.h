#pragma once
#include "primitive.h"
class scene {
public:
	void add(const mesh *m) {
		meshes.push_back(m);
	}
	const std::vector<const mesh *> getobjs() const {
		return meshes;
	}
private:
	std::vector<const mesh *> meshes;
};
