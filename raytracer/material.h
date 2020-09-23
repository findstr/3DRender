#pragma once
#include <memory>
#include "type.h"
#include "texture.h"

struct material {
	enum type {
		GLASS,
		GLOSSY,
		SPECULAR,
	};
	material(std::shared_ptr<itexture> &t)
		:type(GLOSSY),
		ior(1.3),
		Kd(0.8),
		Ks(0.2),
		diffuse(0.2, 0.2, 0.2),
		specularexponent(25.f),
		tex(t) {}
	enum type type;
	float ior;
	float Kd;
	float Ks;
	vector3f diffuse;
	float specularexponent;
	std::shared_ptr<itexture> tex;
};

