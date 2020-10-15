#pragma once
#include <cmath>
#include <random>
#include <algorithm>
#include "type.h"

namespace render {

ALL static inline float
clamp(float lo, float hi, float v)
{
	return fmaxf(lo, fminf(hi, v));
}

ALL static inline float
clamp(float v)
{
	return clamp(0.f, 1.f, v);
}

ALL static inline vector3f
lerp(float frac, vector3f &a, vector3f &b)
{
	return a + frac * (b - a);
}

ALL static inline vector3f
lerp(vector3f &a, vector3f &b, float frac)
{
	return a + frac * (b - a);
}

ALL static inline float
deg2rad(float deg)
{
	return deg * PI / 180.f;
}

ALL static inline float
rad2deg(float rad)
{
	return rad * 180.f / PI;
}

ALL static inline vector4f
tovector4f(const vector3f &a, float x)
{
	return  vector4f(a[0], a[1], a[2], x);
}

ALL static inline vector3f
barycentric2d(float x, float y, const vector4f v[3]) {
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

ALL static inline bool
solve_quadratic(float a, float b, float c, float &x0, float &x1)
{
    float discr = b * b - 4.f * a * c;
    if (discr < 0.f)
        return false;
    else if (discr == 0.f)
        x0 = x1 = -0.5f * b / a;
    else
    {
        float q = (b > 0.f) ? -0.5f * (b + sqrt(discr)) : -0.5f * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1)
        std_swap(x0, x1);
    return true;
}

ALL static inline vector3f
tone_mapping(const vector3f &c, float exposure = 1.f)
{
	vector3f one;
	for (int i = 0; i < 3; i++)
		one[i] = 1.f - std::exp(-c[i] * exposure);
	return one;
}

ALL static inline vector3f
spherical2cartesian(float theta, float phi)
{
	float x = sinf(theta) * cosf(phi);
	float y = sinf(theta) * sinf(phi);
	float z = cosf(theta);
	return vector3f(x, y, z);
}

}

