#pragma once
#include <vector>
#include <memory>
#include "primitive.h"

class scene {
public:
	void add(std::unique_ptr<primitive> &p) {
		primitives.emplace_back(std::move(p));
	}
	void add(const light *l) {
		lights.emplace_back(l);
	}
	bool intersect(const ray &r, hit &h) const;
	const std::vector<const light *>getlights() const {return lights;}
private:
	std::vector<std::unique_ptr<primitive>> primitives;
	std::vector<const light *> lights;
};

