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

static inline float
d_ggx_tr(const vector3f &n, const vector3f &h, float roughness)
{
	float a2 = roughness*roughness;
	float n_dot_h = std::max(n.dot(h), 0.f);
	float n_dot_h2 = n_dot_h * n_dot_h;

	float nom = a2;
	float denom = n_dot_h2 * (a2 - 1.f) + 1.f;

	return nom / (PI * denom * denom);
}

static inline float
g_schlick_ggx(const vector3f &n, const vector3f &v,
	const vector3f &l, float roughness)
{
	float r = roughness + 1.f;
	float k = (r*r) / 8.f;
	float n_dot_v = std::max(n.dot(v), 0.f);
	float n_dot_l = std::max(n.dot(l), 0.f);
	float g1 = n_dot_v / (n_dot_v * (1.f - k) + k);
	float g2 = n_dot_l / (n_dot_l * (1.f - k) + k);
	return g1*g2;
}

static inline vector3f
f_schlick(const vector3f &v, const vector3f &l, const vector3f &f0)
{
	vector3f h = (v+l).normalized();
	float h_dot_v = 1.f - h.dot(v);
	return f0 + (vector3f(1.f, 1.f, 1.f) - f0) * std::pow(h_dot_v, 5);
}



}

