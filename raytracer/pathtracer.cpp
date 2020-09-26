#include <omp.h>
#include <thread>
#include <unistd.h>
#include "optics.h"
#include "auxiliary.h"
#include "pathtracer.h"

vector3f
pathtracer::specular(const ray &r, const hit &h, int depth)
{
	return vector3f(0,0,0);
}

vector3f
pathtracer::glass(const ray &r, const hit &h, int depth)
{
	return vector3f(0,0,0);
}

vector3f
pathtracer::glossy(const ray &r, const hit &h, int depth)
{
	return vector3f(0,0,0);
}

vector3f
pathtracer::light(const ray &r, const hit &h, int depth)
{
	return depth == 0 ? h.obj->material()->emission : vector3f(0,0,0);
}

vector3f
pathtracer::brdf(const hit &h, const vector3f &wi, const vector3f &wo, float a)
{
	auto &N = h.normal;
	auto m = h.obj->material();
	switch(m->type) {
	case material::DIFFUSE:
		// calculate the contribution of diffuse   model
		if (wo.dot(N) > 0.f) {
			auto color = m->texture->sample(h.texcoord);
			return 1.f / PI * color;
		}
		break;
	case material::SPECULAR:
		if (wo.dot(N) > 0.f) {
			float roughness = 0.025f;
			float metallic = 0.9f;
			vector3f x0(0.04, 0.04, 0.04);
			vector3f f0(0.17, 0.17, 0.17);
			f0 = lerp(0.9, x0, f0);
			auto H = (wi + wo).normalized();
			float D = optics::d_ggx_tr(N, H, 0.025f);
			float G = optics::g_schlick_ggx(N, wo, wi, 0.025f);
			vector3f F = optics::f_schlick(wo, wi, f0);
			vector3f specular = (D*G*F) / (4*wo.dot(N)*std::max(wi.dot(N),0.f) + EPSILON);
			vector3f albedo = m->texture->sample(h.texcoord) / PI;
			vector3f Ks = F;
			vector3f Kd = vector3f(1.f, 1.f, 1.f) - Ks;
			Kd *= 1.f - metallic;
			/*
			std::cout << "Ks:" << F << std::endl;
			std::cout << "D:" << D << "G:" << G << "F:" << F << std::endl;
			std::cout << "spe:" << specular << std::endl;
			std::cout << "albedo:" << albedo << std::endl;
			std::cout << "ax:" << Kd.cwiseProduct(albedo) << std::endl;
			std::cout << "sx:" << Ks.cwiseProduct(specular) << std::endl;
			*/
			return Kd.cwiseProduct(albedo) + specular;
		}
		break;
	}
	return vector3f(0,0,0);
}

float
pathtracer::pdf(const material *m, const vector3f &wi,
	const vector3f &wo, const vector3f &N)
{
	switch (m->type) {
	case material::SPECULAR:
	case material::DIFFUSE:
		// uniform sample probability 1 / (2 * PI)
		if (wi.dot(N) > 0.f)
			return 0.5f / PI;
		break;
	}
	return 0.f;
}

static vector3f toWorld(const vector3f &a, const vector3f &N)
{
        vector3f B, C;
        if (std::fabs(N.x()) > std::fabs(N.y())){
            float invLen = 1.0f / std::sqrt(N.x() * N.x() + N.z() * N.z());
            C = vector3f(N.z() * invLen, 0.0f, -N.x() *invLen);
        }
        else {
            float invLen = 1.0f / std::sqrt(N.y() * N.y() + N.z() * N.z());
            C = vector3f(0.0f, N.z() * invLen, -N.y() *invLen);
        }
        B = C.cross(N);
        return a.x() * B + a.y() * C + a.z() * N;
}

vector3f
pathtracer::sample_uniform(const material *m,
	const vector3f &wo, const vector3f &N)
{
	switch(m->type) {
	case material::SPECULAR:
	case material::DIFFUSE:{
		// uniform sample on the hemisphere
		float x_1 = randomf(), x_2 = randomf();
		float z = std::fabs(1.0f - 2.0f * x_1);
		float r = std::sqrt(1.0f - z * z), phi = 2 * PI * x_2;
		vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);
		return toWorld(localRay, N).normalized();}
	default:
		assert(0);
		break;
	}
	return vector3f(0,0,0);
}


vector3f
pathtracer::diffuse(const ray &r, const hit &h, int depth)
{
	float light_pdf;
	vector3f L_dir(0,0,0), L_indir(0,0,0);
	auto &N = h.normal;
	auto wo = -r.direction;
	auto m = h.obj->material();
	{
		hit hit_l, hit_middle;
		light_pdf = sc->samplelight(hit_l);
		auto dir = hit_l.point - h.point;
		auto wi = dir.normalized();
		auto eye = h.point + wi * EPSILON;
		ray r(eye, wi);
		sc->intersect(r, hit_middle);
	//	if (hit_middle.obj == hit_l.obj) { //has no middle
		if (hit_middle.distance - dir.norm() > -0.001f) { //has no middle
			auto L_i = hit_l.obj->material()->emission;
			auto f_r = brdf(h, wi, wo);
			auto cos = std::max(N.dot(wi), 0.f);
			auto cos_prime = std::max(hit_l.normal.dot(-wi), 0.f);
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
			//std::cout << " Ldir:" << L_dir << std::endl;
		}
	}
	if (1){
		float ksi = randomf();
		if (ksi < 0.8) {
			auto wi = sample_uniform(m, wo, N);
			float pdf_ = pdf(m, wi, wo, N) + EPSILON;
			auto f_r = brdf(h, wi, wo);
			ray rr(h.point + EPSILON * wi, wi);
			auto L_i = trace(rr, depth + 1);
			auto cos = std::max(N.dot(wi), 0.f);
			L_indir = L_i.cwiseProduct(f_r) * cos / (pdf_ * 0.8);
		}
	}

	return L_dir + L_indir;
}

vector3f
pathtracer::trace(ray r, int depth)
{
	int i;
	hit h;
	vector3f hitcolor(0,0,0);
	if (!sc->intersect(r, h))
		return hitcolor;
	switch (h.obj->material()->type) {
	case material::LIGHT:
		hitcolor = light(r, h, depth);
		break;
	case material::SPECULAR:
	case material::DIFFUSE:
		hitcolor = diffuse(r, h, depth);
		break;
	case material::GLASS:
		hitcolor = glass(r, h, depth);
		break;
	case material::GLOSSY:
		hitcolor = glossy(r, h, depth);
		break;
	default:
		return vector3f(0,0,0);
	}
	for (int i = 0; i < 3; i++)
		hitcolor[i] = std::max(hitcolor[i], 0.f);
	return hitcolor;
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

void thread_progress()
{
	while (progress < total) {
       		UpdateProgress((float)progress / (float)total);
                usleep(1000);
        }
}


void
pathtracer::render(const scene &sc, screen &scrn)
{
	this->sc = &sc;
	this->scrn = &scrn;
	auto &size = scrn.getsize();
	float scale = std::tan(deg2rad(fov * 0.5f));
	float aspect = (float)size.x() / (float)size.y();
#if 0
	float d = 1.0f;
	float t = d * scale;
	float b = -t;
	float r = t * aspect;
	float l = -r;
	vector3f u(1.f, 0.f, 0.f);
	vector3f v(0.f, 1.f, 0.f);
	vector3f w(0.f, 0.f, -1.f);
	// Use this variable as the eye position to start your rays.
#endif
	int spp = 32;
	progress = 0;
	std::cout << "spp:" << spp << std::endl;
	total = size.x() * size.y() * spp;
	int width = size.x();
	int height = size.y();
	std::thread thread(thread_progress);
	#pragma omp parallel for
	for (uint64_t n = 0; n < total; n++) {
		int w = n/spp;
		uint32_t j = w / width;
		uint32_t i = w % width;
		// generate primary ray direction
		float x = (2 * (i + randomf()) / (float)width - 1) * aspect * scale;
		float y = (2 * (j + randomf()) / (float)height - 1) * scale;
		vector3f dir = vector3f(-x, y, 1).normalized();
		auto c = trace(ray(eye_pos, dir), 0);
		scrn.add(i, j, tone_mapping(c));
		#pragma omp atomic
		++progress;
	}
	thread.join();
	float frac = 1.f / (float)spp;
	for (int j = 0; j < size.y(); ++j) {
		for (int i = 0; i < size.x(); ++i) {
			scrn.mul(i, j, frac);
		}
	}
	UpdateProgress(1.f);
}

