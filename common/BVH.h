#pragma once
#include <memory>
#include "AABB.h"

class BVH {
public:
	struct node {
		AABB3f bounds;
		std::vector<const AABB3f *> objs;
		std::unique_ptr<node> left = nullptr;
		std::unique_ptr<node> right = nullptr;
	};
public:
	BVH(std::vector<AABB3f> &b);
	bool intersect(const ray &r, std::vector<int> &result);
private:
	void intersect_r(node *root, const ray &r, std::vector<int> &result);
	node *build(std::vector<const AABB3f *> &objs, int depth = 0);
private:
	std::unique_ptr<node> root;
	std::vector<AABB3f> buf;
};
