#include "auxiliary.h"
#include "scene.h"

void
scene::add(std::unique_ptr<primitive> &p)
{
	auto *ptr = p.get();
	primitives.emplace_back(std::move(p));
	if (ptr->material()->type == material::LIGHT)
		lights.emplace_back(ptr);
}

bool
scene::intersect(const ray &r, hit &h) const
{
	for (auto &p:primitives) {
		hit th;
		if (p->intersect(r, th) && th.distance < h.distance)
			h = th;
	}
	return h.obj != nullptr;
}

float
scene::samplelight(hit &h) const
{
	float emit_area_sum = 0.f;
	for (auto l:lights)
		emit_area_sum += l->area();
	float p = randomf() * emit_area_sum;
	emit_area_sum = 0.f;
	for (auto l:lights) {
		emit_area_sum += l->area();
		if (emit_area_sum >= p)
			return l->sample(h);
	}
	assert(!"never come here");
	return 0.f;
}

void
scene::dump()
{
	for (auto &p:primitives) {
		p->material()->dump();
	}
}

