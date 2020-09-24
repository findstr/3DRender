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
		vector3f I = li->material()->emission;
		vector3f h = (v+l).normalized();
		//compute light
		ray shadowray(shadow_pos, l);
		if (!sc->intersect(shadowray, hs) || hs.distance >= r)
			ambient += I * std::max(0.f, l.dot(N));
		float p = m->specularexponent;
		specular += I * std::pow(std::max(0.f, h.dot(N)), p);
	}
	vector3f diffuse = m->texture->sample(h.texcoord);
	return ambient.cwiseProduct(diffuse) * m->Kd + specular * m->Ks;
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
	case material::SPECULAR:
		return specular(r, h, depth);
	case material::GLASS:
		return glass(r, h, depth);
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

void
raytracer::render(const scene &sc, screen &scrn)
{
	this->sc = &sc;
	this->scrn = &scrn;
	auto &size = scrn.getsize();
	float scale = std::tan(deg2rad(fov * 0.5f));
	float aspect = (float)size.x() / (float)size.y();

	float d = 1.0f;
	float t = d * scale;
	float b = -t;
	float r = t * aspect;
	float l = -r;
	vector3f u(1.f, 0.f, 0.f);
	vector3f v(0.f, 1.f, 0.f);
	vector3f w(0.f, 0.f, -1.f);
	// Use this variable as the eye position to start your rays.
	vector3f color;
	vector3f eye_pos(0, 0, 0);
	int m = 0;
	for (int j = 0; j < size.y(); ++j) {
		for (int i = 0; i < size.x(); ++i) {
			float x = l + (r - l)*(i + 0.5f) / size.x();
			float y = b + (t - b)*(j + 0.5f) / size.y();
			vector3f dir = (x * u + y * v + w).normalized();
			color = trace(ray(eye_pos, dir), 0);
			scrn.set(i, j, color);
		}
		UpdateProgress((float)j  / size.y());
	}
}

