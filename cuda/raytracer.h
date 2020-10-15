#pragma once
#include "type.h"
#include "screen.h"
#include "camera.h"
#include "scene.h"
#include "primitive.h"

namespace render {

class raytracer : public gpu {
public:
	enum mode {
		RAYTRACING,
		PATHTRACING,
	};
public:
	CPU raytracer(const camera &c, enum mode m = RAYTRACING, vector3f bg = vector3f(0,0,0));
	CPU void setmode(enum mode m);
	CPU void setbackground(vector3f c);
	GPU void render(thread_ctx *ctx, const scene &sc, screen &scrn);
	//GPU bool render(const scene &sc, screen &scrn, int spp = 128);
private:
	GPU vector3f trace(thread_ctx *ctx, ray r, int depth);
	GPU vector3f raytracing(thread_ctx *ctx, const ray &r, const hit &h, int depth);
	GPU vector3f pathtracing(thread_ctx *ctx, const ray &r, const hit &h, int depth);
private: //material
	GPU vector3f light(const ray &r, const hit &h, int depth);
	GPU vector3f glass(thread_ctx *ctx, const ray &r, const hit &h, int depth);
	GPU vector3f diffuse(thread_ctx *ctx, const ray &r, const hit &h, int depth);
private:
	enum mode mode_;
	const camera &camera_;
	const scene *sc = nullptr;
	vector3f background = vector3f(0, 0, 0);
};

GPU vector3f
raytracer::glass(thread_ctx *ctx, const ray &r, const hit &h, int depth)
{
	auto &dir = r.direction;
	auto &N = h.normal;
	auto m = materialX(h.obj);
	auto hit_point = r.move(h.distance);
	vector3f reflect_dir = optics::reflect(dir, N).normalized();
	vector3f refract_dir = optics::refract(dir, N, m->ior).normalized();
	float reflect_offset = reflect_dir.dot(N) >= 0 ? EPSILON : -EPSILON;
	float refract_offset = refract_dir.dot(N) >= 0 ? EPSILON : -EPSILON;
	ray reflect_ray(hit_point + reflect_offset * N, reflect_dir);
	ray refract_ray(hit_point + refract_offset * N, refract_dir);
	vector3f reflect_color, refract_color;
	reflect_color = trace(ctx, reflect_ray, depth + 1);
	refract_color = trace(ctx, refract_ray, depth + 1);
	float kr = optics::fresnel(dir, N, m->ior);
	return reflect_color * kr + refract_color * (1 - kr);
}

GPU vector3f
raytracer::diffuse(thread_ctx *ctx, const ray &r, const hit &h, int depth)
{
#if 0
	auto m = h.obj->material();
	auto &N = h.normal;
	auto &dir = r.direction;
	auto hit_point = r.move(h.distance);
	vector3f ambient(0,0,0), specular(0,0,0);
	float sign = dir.dot(N) < 0 ? 1.f : -1.f;
	vector3f shadow_pos = hit_point + sign * N * EPSILON;
	for (auto li:sc->getlights()) {
		hit hs;
		float r = (li->position() - hit_point).norm();
		vector3f l = (li->position() - hit_point).normalized();
		vector3f v = -dir;
		vector3f I = li->material()->albedo(h.texcoord);
		vector3f h = (v+l).normalized();
		//compute light
		ray shadowray(shadow_pos, l);
		if (!sc->intersect(shadowray, hs) || hs.distance >= r || hs.obj == li)
			ambient += I * std::max(0.f, l.dot(N));
		specular += I * std::pow(std::max(0.f, h.dot(N)), 25.f);
	}
	vector3f diffuse = m->albedo(h.texcoord);
	return ambient.cwiseProduct(diffuse) * m->Kd() + specular * m->Ks();
#endif
	return vector3f(0,0,0);
}

GPU vector3f
raytracer::light(const ray &r, const hit &h, int depth)
{
	return depth == 0 ? materialX(h.obj)->albedo(h.texcoord): vector3f(0,0,0);
}

GPU vector3f
raytracer::pathtracing(thread_ctx *ctx, const ray &rr, const hit &hh, int depth)
{
	vector3f L(0,0,0);
	vector3f frac(1,1,1);
	ray r = rr;
	hit h = hh;
	for (int i = 0; i < 10; i++) {
		auto &N = h.normal;
		auto wo = -r.direction;
		auto m = materialX(h.obj);
		if (m->type == material::LIGHT)
			return light(r, h, depth);
		{
		vector3f L_dir(0,0,0);
		hit hit_l, hit_middle;
		float light_pdf = sc->samplelight(ctx, hit_l);
		auto dir = hit_l.point - h.point;
		auto wi = dir.normalized();
		auto eye = h.point + wi * EPSILON;
		ray r(eye, wi);
		sc->intersect(r, hit_middle);
	//	if (hit_middle.obj == hit_l.obj) { //has no middle
		if (hit_middle.distance - dir.norm() > -0.001f) { //has no middle
			auto L_i = materialX(hit_l.obj)->albedo(hit_l.texcoord);
			auto f_r = m->brdf(h, wi, wo);
			auto cos = fmaxf(N.dot(wi), 0.f);
			auto cos_prime = fmaxf(hit_l.normal.dot(-wi), 0.f);
			auto r_square = dir.squaredNorm();
			light_pdf += EPSILON;
			/*
			std::cout << "f_r:" << f_r << std::endl;
			std::cout << "L_i:" << L_i << std::endl;
			std::cout << "cos_prim:" << cos_prime << std::endl;
			std::cout << "cos:" << cos << std::endl;
			std::cout << " demo:" << (r_square * light_pdf) << std::endl;
			*/
			L_dir = L_i.cwiseProduct(f_r) * cos_prime * cos / (r_square * light_pdf);
			L += L_dir.cwiseProduct(frac);
			//std::cout << " Ldir:" << L_dir << std::endl;
		}
		}
		if (1) {
		float ksi = ctx->rand();
		if (ksi < 0.8) {
			vector3f L_indir(0,0,0);
			auto wi = m->sample(ctx, wo, N);
			float pdf_ = m->pdf(wi, wo, N) + EPSILON;
			auto f_r = m->brdf(h, wi, wo);
			auto cos = fmaxf(N.dot(wi), 0.f);
			auto f = f_r * cos / (pdf_ * 0.8f);
			frac = frac.cwiseProduct(f);
			ray rx(h.point + EPSILON * wi, wi);
			r = rx;
			depth += 1;
			if (!sc->intersect(r, h)) {
				L += background.cwiseProduct(frac);
				break;
			}
		} else {
			break;
		}
		}
	}
	return L;
}

GPU vector3f
raytracer::raytracing(thread_ctx *ctx, const ray &r, const hit &h, int depth)
{
	vector3f hitcolor(0,0,0);
	if (depth > 6)
		return hitcolor;
	switch (materialX(h.obj)->type) {
	case material::LIGHT:
		return light(r, h, depth);
	case material::GLASS:
		return glass(ctx, r, h, depth);
	case material::DIFFUSE:
		return diffuse(ctx, r, h, depth);
	default:
		return vector3f(0,0,0);
	}
}

GPU vector3f
raytracer::trace(thread_ctx *ctx, ray r, int depth)
{
	hit h;
	if (!sc->intersect(r, h))
		return background;
//	if (mode_ == RAYTRACING)
//		return raytracing(ctx, r, h, depth);
//	else
	vector3f c = pathtracing(ctx, r, h, depth);
	return c;
}

GPU void
raytracer::render(thread_ctx *ctx, const scene &sc, screen &scrn)
{
	int i = ctx->x;
	int j = ctx->y;
	this->sc = &sc;
	int width = scrn.getsize().x();
	int height = scrn.getsize().y();
	float aspect = scrn.aspect();
	float x = (i + 0.5f) / (float)width;
	float y = (j + 0.5f) / (float)height;
	ray r = camera_.lookat(aspect, x, y);
	auto c = tone_mapping(trace(ctx, r, 0));
	scrn.add(i, j, c);
}

#if 0
GPU bool
raytracer::render(const scene &sc, screen &scrn, int spp)
{
	this->sc = &sc;
	auto &size = scrn.getsize();
	float aspect = scrn.aspect();
	int width = size.x();
	int height = size.y();
	int total = (uint64_t)width * height;
	#pragma omp parallel for
	for (uint64_t n = 0; n < total; n++) {
		uint64_t w = n;
		uint32_t i = int(w % width);
		uint32_t j = int(w / width);
		float x = (i + 0.5f) / (float)width;
		float y = (j + 0.5f) / (float)height;
		ray r = camera_.lookat(aspect, x, y);
		auto c = tone_mapping(trace(ctx, r, 0));
		scrn.add(i, j, c);
	}
	return true;
}
#endif

CPU void
raytracer::setmode(enum mode m)
{
	mode_ = m;
}

CPU void
raytracer::setbackground(vector3f c)
{
	background = c;
}

CPU 
raytracer::raytracer(const camera &c, raytracer::mode m, vector3f bg):
	mode_(m),camera_(c), background(bg)
{
}


}
