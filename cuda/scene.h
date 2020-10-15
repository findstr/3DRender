#pragma once
#include <vector>
#include <memory>
#include "type.h"
#include "primitive.h"

namespace render {

class scene : public gpu {
public:
	CPU void add(std::vector<std::unique_ptr<primitive>> &p);
	GPU float samplelight(thread_ctx *ctx, hit &h) const;
	GPU bool intersect(const ray &r, hit &h) const;
	GPU const gpu_vector<const primitive *> &getlights() const {return lights;}
	CPU void dump();
private:
	gpu_vector<const primitive *> primitives;
	gpu_vector<const primitive *> lights;
};

GPU bool
scene::intersect(const ray &r, hit &h) const
{
	for (int i = 0; i < primitives.size(); i++) {
		hit th;
		auto p = primitives[i];
		if (intersectX(p, r, th) && th.distance < h.distance)
			h = th;
	}
	return h.obj != nullptr;
}



GPU float
scene::samplelight(thread_ctx *ctx, hit &h) const
{
	float emit_area_sum = 0.f;
	for (int i = 0; i < lights.size(); i++) {
		auto l = lights[i];
		emit_area_sum += areaX(l);
	}
	float p = ctx->rand() * emit_area_sum;
	emit_area_sum = 0.f;
	for (int i = 0; i < lights.size(); i++) {
		auto l = lights[i];
		emit_area_sum += areaX(l);
		if (emit_area_sum >= p)
			return sampleX(l, ctx, h);
	}
	assert(!"never come here");
	return 0.f;
}


CPU void
scene::add(std::vector<std::unique_ptr<primitive>> &px)
{
	cpu_vector<const primitive *> pri;
	cpu_vector<const primitive *> li;
	for (auto &p:px) {
		auto ptr = p.get();
		pri.emplace_back(ptr);
	}
	const primitive *x = (const primitive *)primitives.data();
	for (auto &p:pri) {
		if (materialX(p)->type == material::LIGHT)
			li.emplace_back(p);
	}
	lights = li;
	primitives = pri;
}


void
scene::dump()
{
	cpu_vector<const primitive *> pri = primitives;
	for (auto &p:pri) {
		materialX(p)->dump();
	}
}


}

