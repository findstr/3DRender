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
		SPECULAR,
	};
	material(std::shared_ptr<itexture> &t, type typ = DIFFUSE)
		:type(typ),
		ior(1.3),
		Kd(1.0),
		Ks(0.2),
		emission(0,0,0),
		specularexponent(25.f),
		texture(t) {}
	material(const vector3f &e, type typ = LIGHT)
		:type(typ),
		ior(1.3),
		Kd(1.0),
		Ks(0.2),
		emission(e),
		specularexponent(25.f),
		texture(nullptr) {}

	enum type type;
	float ior;
	float Kd;
	float Ks;
	vector3f emission;
	float specularexponent;
	std::shared_ptr<itexture> texture;
};

