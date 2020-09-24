#pragma once
#include <vector>
#include <memory>
#include "primitive.h"

class scene {
public:
	void add(std::unique_ptr<primitive> &p);
	float samplelight(hit &h) const;
	bool intersect(const ray &r, hit &h) const;
	const std::vector<const primitive *>getlights() const {return lights;}
private:
	std::vector<std::unique_ptr<primitive>> primitives;
	std::vector<const primitive *> lights;
};

