#include "optics.h"
#include "auxiliary.h"
#include "material.h"

vector3f
material::albedo(const vector2f &texcoord) const
{
	auto *tex = texture_.get();
	if (tex != nullptr)
		return albedo_ + tex->sample(texcoord);
	else
		return albedo_;
}

float
material::Kd() const
{
	return roughness_or_kd;
}

float
material::Ks() const
{
	return metallic_or_ks;
}

vector3f
material::brdf(const hit &h, const vector3f &wi, const vector3f &wo) const
{
	auto &N = h.normal;
	switch(this->type) {
	case material::DIFFUSE:
		// calculate the contribution of diffuse   model
		if (wo.dot(N) > 0.f) {
			auto color = this->albedo(h.texcoord);
			return 1.f / PI * color;
		}
		break;
	case material::MICROFACET: {
		if (wo.dot(N) > 0.f) {
			vector3f x0(0.04, 0.04, 0.04);
			vector3f albedo = this->albedo(h.texcoord);
			auto f0 = lerp(x0, albedo, metallic_or_ks);
			auto H = (wi + wo).normalized();
			float D = optics::d_ggx_tr(N, H, 0.025f);
			float G = optics::g_schlick_ggx(N, wo, wi, 0.025f);
			vector3f F = optics::f_schlick(wo, wi, f0);
			vector3f specular = (D*G*F) / (4*std::max(wo.dot(N), 0.f)*std::max(wi.dot(N),0.f) + EPSILON);
			vector3f Ks = F;
			vector3f Kd = vector3f(1.f, 1.f, 1.f) - Ks;
			Kd *= 1.f - metallic_or_ks;
			return Kd.cwiseProduct(albedo)/PI + specular;
		}
		break;}
	}
	return vector3f(0,0,0);
}

float
material::pdf(const vector3f &wi, const vector3f &wo, const vector3f &N) const
{
	switch (this->type) {
	case material::DIFFUSE:
		// uniform sample probability 1 / (2 * PI)
		if (wi.dot(N) > 0.f)
			return 0.5f / PI;
		break;
	case material::MICROFACET: {
		vector3f H = (wi+wo).normalized();
		float a2 = roughness_or_kd * roughness_or_kd;
		float costheta = N.dot(H);
		float exp = (a2 - 1) * costheta * costheta + 1.f;
		float D = a2 / (PI * exp * exp);
		return (D * costheta) / (4 * wo.dot(H));
		break;}
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
material::sample(const vector3f &wo, const vector3f &N) const
{
	switch(this->type) {
	case material::DIFFUSE:{
		// uniform sample on the hemisphere
		float x_1 = randomf(), x_2 = randomf();
		float z = std::fabs(1.0f - 2.0f * x_1);
		float r = std::sqrt(1.0f - z * z), phi = 2 * PI * x_2;
		vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);
		return toWorld(localRay, N).normalized();}
	case material::MICROFACET: {//sample GGX
		float r0 = randomf();
		float r1 = randomf();
		float a2 = roughness_or_kd * roughness_or_kd;
		float theta = acosf(sqrtf((1.f - r0) / ((a2 - 1.f) * r0 + 1)));
		float phi = 2 * PI * r1;
		vector3f wm = spherical2cartesian(theta, phi);
		vector3f wm_w = toWorld(wm, N);
		vector3f wi = 2 * wm_w * wm_w.dot(wo) - wo;
		return wi.normalized();
	}
	default:
		assert(0);
		break;
	}
	return vector3f(0,0,0);
}



void
material::dump() const
{
	std::cout << "ior:" << ior <<
		"type:" << type <<
		"roughness:" << roughness_or_kd <<
		"metallic:" << metallic_or_ks <<
		"albedo:" << albedo_.transpose() <<
		" tex:" << texture_.get() << std::endl;
}



