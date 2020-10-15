#pragma once

#define ENABLE_GPU

#ifndef PI
#define PI	(3.14159265358979323846f)
#endif
#ifndef EPSILON
#define EPSILON	(0.00001f)
#endif

#ifdef _MSC_VER
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#endif

#include "gpu.h"
#include "matrix.h"
