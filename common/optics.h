#pragma once
#include "type.h"
#include "auxiliary.h"

namespace optics {

static inline vector3f
reflect(const vector3f &I, const vector3f &N)
{
	return I - 2 * I.dot(N) * N;
}

static inline vector3f
refract(const vector3f &I, const vector3f &N, float ior)
{
	float cosi = clamp(-1, 1, I.dot(N));
	float etai = 1, etat = ior;
	vector3f n = N;
	if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? vector3f(0,0,0) : eta * I + (eta * cosi - sqrtf(k)) * n;
}

static inline float
fresnel(const vector3f &I, const vector3f &N, float ior)
{
	float cosi = clamp(-1, 1, I.dot(N));
	float etai = 1, etat = ior;
	if (cosi > 0) {  std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		return 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}


}

