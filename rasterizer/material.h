#pragma once
#include "type.h"
#include "texture.h"

struct ishader {
	vector3f view_pos;
	vector3f color;
	vector3f normal;
	vector2f texcoord;
	texture *tex;
	virtual vector3f frag() const = 0;
};

class normal_shader : public ishader {
public:
	vector3f frag() const override {
		return normal.normalized() + vector3f(1.f, 1.f, 1.f) / 2.f;
	}
};

struct light
{
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
};


class texture_shader : public ishader {
public:
	vector3f frag() const override {
		//TODO: move ka kd ks to material
		auto return_color = tex->sample(texcoord);
		vector3f texture_color;
		texture_color << return_color.x(), return_color.y(), return_color.z();

		vector3f ka = vector3f(0.005, 0.005, 0.005);
		vector3f kd = texture_color;
		vector3f ks = vector3f(0.7937, 0.7937, 0.7937);

		auto l1 = light{{20, 20, 20}, {500, 500, 500}};
		auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

		std::vector<light> lights = {l1, l2};
		vector3f amb_light_intensity{10, 10, 10};
		vector3f eye_pos{0, 0, 10};

		float p = 150;

		vector3f point = view_pos;
		vector3f n = normal.normalized();
		vector3f result_color = {0, 0, 0};
		for (auto& light : lights)
		{
			float r = (light.position - point).norm();
			vector3f l = (light.position - point).normalized();
			vector3f v = (eye_pos - point).normalized();
			vector3f I = light.intensity/(r*r);
			vector3f h = (l + v).normalized();
			//diffuse
			vector3f Ld, Ls(0,0,0), La(0,0,0);
			Ld = kd.cwiseProduct(I * std::max(n.dot(l), 0.f));
			Ls = ks.cwiseProduct(I * std::pow(std::max(0.f, n.dot(h)), p));
			La = ka.cwiseProduct(amb_light_intensity);
			result_color += Ld + Ls + La;
		}
		return result_color;
	}
};

class material {
public:
	material(ishader *s, texture *t):shader(s), tex(t) {}
	texture *tex = nullptr;
	ishader *shader = nullptr;
};

