#pragma once
#include <cmath>
#include <random>
#include "type.h"


static inline float
clamp(float lo, float hi, float v)
{
	return std::max(lo, std::min(hi, v));
}

static inline float
clamp(float v)
{
	return clamp(0.f, 1.f, v);
}

static inline vector3f
lerp(float frac, vector3f &a, vector3f &b)
{
	return a + frac * (b - a);
}

static inline vector3f
lerp(vector3f &a, vector3f &b, float frac)
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

static inline bool
solve_quadratic(float a, float b, float c, float &x0, float &x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0)
        x0 = x1 = -0.5 * b / a;
    else
    {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1)
        std::swap(x0, x1);
    return true;
}


float randomf();

static inline vector3f
tone_mapping(const vector3f &c, float exposure = 1.f)
{
	vector3f one;
	for (int i = 0; i < 3; i++)
		one[i] = 1.f - std::exp(-c[i] * exposure);
	return one;
}

static inline vector3f
spherical2cartesian(float theta, float phi)
{
	float x = sinf(theta) * cosf(phi);
	float y = sinf(theta) * sinf(phi);
	float z = cosf(theta);
	return vector3f(x, y, z);
}

