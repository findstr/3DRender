#include <vector>
#include <cassert>
#include <algorithm>
#include "BVH.h"

BVH::BVH(std::vector<AABB3f> &b)
{
	time_t start, stop;
	std::vector<const AABB3f *> objs;
	buf = std::move(b);
	buf.shrink_to_fit();
	objs.reserve(buf.size());
	const AABB3f *ptr = &buf[0];
	for (size_t i = 0; i < buf.size(); i++)
		objs.emplace_back(&ptr[i]);
	time(&start);
	root.reset(build(objs));
	time(&stop);
	double diff = difftime(stop, start);
	int h = (int)diff / 3600;
	int m = ((int)diff / 60) - (h* 60);
	int s = (int)diff - (h * 3600) - (m * 60);
	std::cout << "Build BVH takes:" << h <<
		":" << m << ":" << s << std::endl;
}

BVH::node *
BVH::build(std::vector<const AABB3f *> &objs, int depth)
{
	BVH::node *root = new BVH::node();
	auto &bounds = root->bounds;
	for (auto ptr:objs)
		bounds.extend(*ptr);
	if (objs.size() < 5) {
		root->left = nullptr;
		root->right = nullptr;
		root->objs = objs;
		return root;
	}
	int dim = 2;
	auto diagonal = bounds.max - bounds.min;
	if (diagonal.x() > diagonal.y() && diagonal.x() > diagonal.z())
		dim = 0;
	else if (diagonal.y() > diagonal.z())
		dim = 1;
	std::sort(objs.begin(), objs.end(), [dim](auto a, auto b) {
		return a->centroid()[dim] < b->centroid()[dim];
	});

	auto b = objs.begin();
	auto m = b + (objs.size() / 2);
	auto e = objs.end();

	auto left = std::vector<const AABB3f *>(b, m);
	auto right = std::vector<const AABB3f *>(m, e);
	root->left.reset(build(left, depth + 1));
	root->right.reset(build(right, depth + 1));
	return root;
}

void
BVH::intersect_r(BVH::node *root, const ray &r, std::vector<int> &result)
{
	if (!root->bounds.intersect(r))
		return ;
	if (root->left.get() == nullptr) {
		assert(root->right.get() == nullptr);
		const AABB3f *start = &buf[0];
		for (auto ptr:root->objs)
			result.emplace_back(ptr - start);
		return ;
	}
	intersect_r(root->left.get(), r, result);
	intersect_r(root->right.get(), r, result);
}

bool
BVH::intersect(const ray &r, std::vector<int> &result)
{
	result.clear();
	intersect_r(root.get(), r, result);
	return result.size();
}

