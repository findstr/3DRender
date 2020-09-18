#pragma once
#include "type.h"

static inline float
clamp(const float &lo, const float &hi, const float &v)
{
	return std::max(lo, std::min(hi, v));
}

static inline float
clamp(float &v)
{
	return clamp(0.f, 1.f, v);
}

static inline vector3f
lerp(float frac, vector3f &a, vector3f &b)
{
	return a + frac * (b - a);
}

static inline float
deg2rad(float deg)
{
	return deg * PI / 180.f;
}

static inline float
rad2deg(float rad)
{
	return rad * 180.f / PI;
}

static inline vector4f
tovector4f(const vector3f &a, float x)
{
	return  vector4f(a[0], a[1], a[2], x);
}

static inline vector3f
barycentric2d(float x, float y, const vector4f v[3]) {
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}



