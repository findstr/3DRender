#include "scene.h"

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

