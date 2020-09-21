#include "bounds.h"

bounds2f::bounds2f()
{
	for (int i = 0; i < 2; i++) {
		min[i] = std::numeric_limits<float>::infinity();
		max[i] = 0;
	};
}


