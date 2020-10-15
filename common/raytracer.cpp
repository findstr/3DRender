#include <omp.h>
#include <thread>
#include "optics.h"
#include "auxiliary.h"
#include "raytracer.h"

vector3f
raytracer::glass(const ray &r, const hit &h, int depth)
{
	auto &dir = r.direction;
	auto &N = h.normal;
	auto m = h.obj->material();
	auto hit_point = r.move(h.distance);
	vector3f reflect_dir = optics::reflect(dir, N).normalized();
	vector3f refract_dir = optics::refract(dir, N, m->ior).normalized();
	float reflect_offset = reflect_dir.dot(N) >= 0 ? EPSILON : -EPSILON;
	float refract_offset = refract_dir.dot(N) >= 0 ? EPSILON : -EPSILON;
	ray reflect_ray(hit_point + reflect_offset * N, reflect_dir);
	ray refract_ray(hit_point + refract_offset * N, refract_dir);
	vector3f reflect_color, refract_color;
	reflect_color = trace(reflect_ray, depth + 1);
	refract_color = trace(refract_ray, depth + 1);
	float kr = optics::fresnel(dir, N, m->ior);
	return reflect_color * kr + refract_color * (1 - kr);
}

vector3f
raytracer::light(const ray &r, const hit &h, int depth)
{
	return depth == 0 ? h.obj->material()->albedo(h.texcoord): vector3f(0,0,0);
}

vector3f
raytracer::pathtracing_r(const ray &rr, const hit &hh, int depth)
{
	vector3f L(0,0,0);
	vector3f frac(1,1,1);
	ray r = rr;
	hit h = hh;
	for (;;) {
		auto &N = h.normal;
		auto wo = -r.direction;
		auto m = h.obj->material();
		if (m->type == material::LIGHT)
			return light(r, h, depth);
		{
		vector3f L_dir(0,0,0);
		hit hit_l, hit_middle;
		float light_pdf = sc->samplelight(hit_l);
		auto dir = hit_l.point - h.point;
		auto wi = dir.normalized();
		auto eye = h.point + N * EPSILON;
		ray r(eye, wi);
		sc->intersect(r, hit_middle);
		if (hit_middle.obj == nullptr || hit_middle.obj == hit_l.obj) { //has no middle
			auto L_i = hit_l.obj->material()->albedo(hit_l.texcoord);
			auto f_r = m->brdf(h, wi, wo);
			auto cos = std::max(N.dot(wi), 0.f);
			auto cos_prime = std::max(hit_l.normal.dot(-wi), 0.f);
			auto r_square = dir.squaredNorm();
			light_pdf += EPSILON;

			L_dir = L_i.cwiseProduct(f_r) * cos_prime * cos / (r_square * light_pdf);
			L += L_dir.cwiseProduct(frac);
		}
		}
		if (1) {
		float ksi = randomf();
		if (ksi < 0.8) {
			vector3f L_indir(0,0,0);
			auto wi = m->sample(wo, N);
			float pdf_ = m->pdf(wi, wo, N) + EPSILON;
			auto f_r = m->brdf(h, wi, wo);
			auto cos = std::max(N.dot(wi), 0.f);
			auto f = f_r * cos / (pdf_ * 0.8f);
			frac = frac.cwiseProduct(f);
			ray rx(h.point + EPSILON * N, wi);
			r = rx;
			depth += 1;
			if (!sc->intersect(r, h))
				break;
		} else {
			break;
		}
		}
	}
	return L;
}

vector3f
raytracer::pathtracing(const ray &r, const hit &h, int depth)
{
	vector3f L(0,0,0);
	auto &N = h.normal;
	auto wo = -r.direction;
	auto m = h.obj->material();
	if (m->type == material::LIGHT)
		return light(r, h, depth);
{
	vector3f L_dir(0,0,0);
	hit hit_l, hit_middle;
	float light_pdf = sc->samplelight(hit_l);
	auto dir = hit_l.point - h.point;
	auto wi = dir.normalized();
	auto eye = h.point + N * EPSILON;
	ray rx(eye, wi);
	sc->intersect(rx, hit_middle);
	if (hit_middle.obj == nullptr || hit_middle.obj == hit_l.obj) { //has no middle
		auto L_i = hit_l.obj->material()->albedo(hit_l.texcoord);
		auto f_r = m->brdf(h, wi, wo);
		auto cos = std::max(N.dot(wi), 0.f);
		auto cos_prime = std::max(hit_l.normal.dot(-wi), 0.f);
		auto r_square = dir.squaredNorm();
		light_pdf += EPSILON;
		L_dir = L_i.cwiseProduct(f_r) * cos_prime * cos / (r_square * light_pdf);
		L += L_dir;
	}
}
if (1) {
	float ksi = randomf();
	if (ksi < 0.8) {
		auto wi = m->sample(wo, N);
		float pdf_ = m->pdf(wi, wo, N) + EPSILON;
		auto f_r = m->brdf(h, wi, wo);
		auto cos = std::max(N.dot(wi), 0.f);
		auto f = f_r * cos / (pdf_ * 0.8f);
		vector3f L_indir;
		ray rx(h.point + EPSILON * N, wi);
		L_indir = trace(rx, depth + 1).cwiseProduct(f);
		L += L_indir;
	}
}
	return L;
}

vector3f
raytracer::raytracing(const ray &r, const hit &h, int depth)
{
	vector3f hitcolor(0,0,0);
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
	hitcolor = ambient.cwiseProduct(diffuse) * m->Kd() + specular * m->Ks();
	return hitcolor;
}

vector3f
raytracer::trace(ray r, int depth)
{
	hit h;
	if (!sc->intersect(r, h)) {
		if (mode_ != RAYTRACING && depth > 0)
			return vector3f(0,0,0);
		else
			return background;
	}
	vector3f hitcolor(0,0,0);
	if (depth > 6)
		return hitcolor;
	switch (h.obj->material()->type) {
	case material::LIGHT:
		return light(r, h, depth);
	case material::GLASS:
		return glass(r, h, depth);
	case material::DIFFUSE:
	case material::MICROFACET:
		if (mode_ == RAYTRACING)
			return raytracing(r, h, depth);
		else
			return pathtracing(r, h, depth);
		//return diffuse(r, h, depth);
	default:
		return vector3f(0,0,0);
	}

}

void
raytracer::render(const scene &sc, screen &scrn)
{
	this->sc = &sc;
	auto &size = scrn.getsize();
	float aspect = scrn.aspect();
	int width = size.x();
	int height = size.y();
	int total = width * height;
	#pragma omp parallel for
	for (int n = 0; n < total; n++) {
		int w = n;
		int i = w % width;
		int j = w / width;
		float x = (i + randomf()) / (float)width;
		float y = (j + randomf()) / (float)height;
		ray r = camera_.lookat(aspect, x, y);
		auto c = tone_mapping(trace(r, 0));
		scrn.add(i, j, c);
	}
	return ;
}

void
raytracer::setmode(enum mode m)
{
	mode_ = m;
}

void
raytracer::setbackground(vector3f c)
{
	background = c;
}

raytracer::raytracer(const camera &c, raytracer::mode m, vector3f bg):
	mode_(m),camera_(c), background(bg)
{
}

