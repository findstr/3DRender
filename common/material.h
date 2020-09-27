#pragma once
#include <memory>
#include "type.h"
#include "texture.h"

struct material {
	enum type {
		GLASS,
		LIGHT,
		GLOSSY,
		DIFFUSE,
		MICROFACET,
	};
	material(std::shared_ptr<texture> t, type typ = DIFFUSE)
		:type(typ),
		ior(1.3),
		roughness_or_kd(0.8),
		metallic_or_ks(0.2),
		albedo_(0,0,0),
		texture_(t) {}
	material(type typ, std::shared_ptr<texture> &tex,
		const vector3f &albedo,
		float roughness, float metallic, float ior_) :
		type(typ), texture_(tex), albedo_(albedo),
		roughness_or_kd(roughness),
		metallic_or_ks(metallic), ior(ior_) {}

	vector3f albedo(const vector2f &texcoord) const {
		auto *tex = texture_.get();
		if (tex != nullptr)
			return albedo_ + tex->sample(texcoord);
		else
			return albedo_;
	}
	float Kd() const {
		return roughness_or_kd;
	}
	float Ks() const {
		return metallic_or_ks;
	}
	void dump() const {
		std::cout << "ior:" << ior <<
			"type:" << type <<
			"roughness:" << roughness_or_kd <<
			"metallic:" << metallic_or_ks <<
			"albedo:" << albedo_.transpose() <<
			" tex:" << texture_.get() << std::endl;
	}
	float ior;
	enum type type;
	float roughness_or_kd;
	float metallic_or_ks;
	vector3f albedo_;
	std::shared_ptr<texture> texture_;
};

