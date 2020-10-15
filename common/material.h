#pragma once
#include <memory>
#include "type.h"
#include "ray.h"
#include "texture.h"

struct material {
	enum type {
		LIGHT,
		GLASS,
		DIFFUSE,
		MICROFACET,
	};
	material(std::shared_ptr<texture> t, type typ = DIFFUSE):
		type(typ),
		ior(1.3f),
		roughness_or_kd(0.8f),
		metallic_or_ks(0.2f),
		albedo_(0,0,0),
		texture_(t) {}
	material(type typ, std::shared_ptr<texture> &tex,
		const vector3f &albedo,
		float roughness, float metallic, float ior_) :
		type(typ),
		ior(ior_),
		roughness_or_kd(roughness),
		metallic_or_ks(metallic),
		albedo_(albedo),
		texture_(tex) {}

	float Kd() const;
	float Ks() const;
	void dump() const;
	vector3f albedo(const vector2f &texcoord) const;
	vector3f sample(const vector3f &wo, const vector3f &N) const;
	float pdf(const vector3f &wi, const vector3f &wo, const vector3f &N) const;
	vector3f brdf(const hit &h, const vector3f &wi, const vector3f &wo) const;

	enum type type;
	float ior;
	float roughness_or_kd;
	float metallic_or_ks;
	vector3f albedo_;
	std::shared_ptr<texture> texture_;
};

