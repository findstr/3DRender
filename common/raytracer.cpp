#include <omp.h>
#include <thread>
#include <unistd.h>
#include "optics.h"
#include "auxiliary.h"
#include "raytracer.h"

vector3f
raytracer::specular(const ray &r, const hit &h, int depth)
{
	vector3f color1, color2;
	auto &dir = r.direction;
	auto &N = h.normal;
	auto m = h.obj->material();
	auto hit_point = r.move(h.distance);
	vector3f reflect_dir = optics::reflect(dir, N).normalized();
	float reflect_offset = reflect_dir.dot(N) < 0 ? -EPSILON : EPSILON;
	ray reflect_ray(hit_point + reflect_offset * N, reflect_dir);
	color1 = trace(reflect_ray, depth + 1);
	color2 = glossy(r, h, depth);
	return color1 * 0.3 + color2 * 0.7;
}

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
raytracer::glossy(const ray &r, const hit &h, int depth)
{
	auto m = h.obj->material();
	auto &N = h.normal;
	auto &dir = r.direction;
	auto hit_point = r.move(h.distance);
	vector3f ambient(0,0,0), specular(0,0,0);
	float sign = dir.dot(N) < 0 ? 1 : -1;
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
}

vector3f
raytracer::trace(ray r, int depth)
{
	int i;
	hit h;
	vector3f hitcolor(0,0,0);
	if (depth > 6) {
		return vector3f(0,0,0);
	}
	if (!sc->intersect(r, h)) {
		return background;
	}
	switch (h.obj->material()->type) {
	case material::MICROFACET:
		return specular(r, h, depth);
	case material::GLASS:
		return glass(r, h, depth);
	case material::DIFFUSE:
	case material::GLOSSY:
		return glossy(r, h, depth);
	default:
		return vector3f(0,0,0);
	}
}

static inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

static int progress = 0;
static int total = 1;

static void thread_progress()
{
	while (progress < total) {
       		UpdateProgress((float)progress / (float)total);
                usleep(1000);
        }
}


void
raytracer::render(const scene &sc, screen &scrn, int spp)
{
	this->sc = &sc;
	auto &size = scrn.getsize();
	float aspect = scrn.aspect();
	int width = size.x();
	int height = size.y();
	std::thread thread(thread_progress);
	float frac = 1.f / (float)spp;
	total = size.x() * size.y() * spp;
	#pragma omp parallel for
	for (uint64_t n = 0; n < total; n++) {
		int w = n/spp;
		uint32_t i = w % width;
		uint32_t j = w / width;
		float x = (i + 0.5f) / (float)width;
		float y = (j + 0.5f) / (float)height;
		ray r = camera_.lookat(aspect, x, y);
		auto c = trace(r, 0) * frac;
		scrn.add(i, j, c);
		#pragma omp atomic
		++progress;
	}
	thread.join();
	UpdateProgress(1.f);
}

raytracer::raytracer(const camera &c):camera_(c) {}

