#pragma once
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene.h"

class scene_loader {
public:
	enum render {
		RASTERIZE,
		RAYTRACING,
		PATHTRACING,
	};
	struct screenst {
		int width;
		int height;
	};
	struct camerast {
		vector3f eye;
		float fov;
		float znear;
		float zfar;
		float yaw;
	};
private:
	int spp = 16;
	int line = 0;
	char type[64];
	enum render rmode;
	screenst screen;
	camerast camera;
	vector3f background;
	std::vector<std::shared_ptr<texture>> textures;
	std::vector<std::shared_ptr<material>> materials;
	std::vector<std::unique_ptr<primitive>> primitives;
private:
	typedef void (*load_t)(const char *line);
	enum material::type materialtype(const char *str) {
		struct entry {
			const char *str;
			enum material::type type;
		} types[] = {
			{"microfacet",	material::MICROFACET},
			{"diffuse", material::DIFFUSE},
			{"light", material::LIGHT},
			{"glass", material::GLASS},
		};
		for (auto &iter:types) {
			if (strcmp(str, iter.str) == 0)
				return iter.type;
		}
		fprintf(stderr, "line:%d incorrect material type:%s\n", line, str);
		exit(0);
	}
	void load_render(const char *buff) {
		char mode[64];
		struct entry {
			const char *str;
			enum render rmode;
		} modes[] = {
			{"rasterize", RASTERIZE},
			{"raytracing", RAYTRACING},
			{"pathtracing", PATHTRACING},
		};
		int n = sscanf(buff, "%*s %s", mode);
		assert(n == 1);
		for (auto &iter:modes) {
			if (strcmp(mode, iter.str) == 0) {
				rmode = iter.rmode;
				return ;
			}
		}
		fprintf(stderr, "line:%d incorrect render mode:%s\n", line, buff);
		exit(0);
	}
	void load_spp(const char *buff) {
		int n = sscanf(buff, "%*s %d", &spp);
		assert(n == 1);
	}
	void load_background(const char *buff) {
		int n = sscanf(buff, "%*s %f,%f,%f",
			&background.x(),
			&background.y(),
			&background.z());
		assert(n == 3);
	}
	void load_screen(const char *buff) {
		int n = sscanf(buff, "%*s %d,%d", &screen.width, &screen.height);
		assert(n == 2);
	}
	void load_camera(const char *buff) {
		auto &c = camera;
		int n =sscanf(buff, "%*s %f,%f,%f %f %f %f %f",
			&c.eye.x(), &c.eye.y(), &c.eye.z(),
			&c.fov, &c.znear, &c.zfar, &c.yaw);
		assert(n == 7);
	}
	void load_texture(const char *line) {
		char path[PATH_MAX];
		int n = sscanf(line, "%s %s", type, path);
		assert(n == 2);
		textures.emplace_back(new texture(path));
	}
	void load_material(const char *buff) {
		char subtype[64];
		enum material::type typ;
		int textureid;
		vector3f albedo;
		float roughness_or_kd;
		float metallic_or_ks;
		float ior = 1.f;
		std::shared_ptr<texture> tex(nullptr);
		int n = sscanf(buff, "%s %s %d %f,%f,%f %f %f %f",
			type, subtype, &textureid,
			&albedo.x(), &albedo.y(), &albedo.z(),
			&roughness_or_kd, &metallic_or_ks, &ior);
		assert(n == 9);
		if (textureid >= 0) {
			if ((int)textures.size() <= textureid) {
				fprintf(stderr, "line:%d incorrect textureid\n", line);
				exit(-1);
			}
			tex = textures[textureid];
		}
		typ = materialtype(subtype);
		materials.emplace_back(new material(typ, tex,
			albedo, roughness_or_kd, metallic_or_ks, ior));
	}
	void load_mesh(const char *buff) {
		char path[PATH_MAX];
		unsigned int materialid;
		int n = sscanf(buff, "%s %s %d", type, path, &materialid);
		assert(n == 3);
		if (materialid >= materials.size()) {
			fprintf(stderr, "line:%d incorrect material id:%d\n", line, materialid);
			exit(0);
		}
		std::unique_ptr<primitive> obj(new mesh(path, materials[materialid]));
		primitives.emplace_back(std::move(obj));
	}
	void load_sphere(const char *buff) {
		vector3f center;
		float radius;
		unsigned int materialid;
		int n = sscanf(buff, "%s %f,%f,%f %f %d", type,
			&center.x(), &center.y(), &center.z(),
			&radius, &materialid);
		assert(n == 6);
		if (materialid >= materials.size()) {
			fprintf(stderr, "line:%d incorrect material id:%d\n", line, materialid);
			exit(0);
		}
		std::unique_ptr<primitive> obj(new sphere(center, radius, materials[materialid]));
		primitives.emplace_back(std::move(obj));
	}
public:
	enum render getrender() {
		return rmode;
	}
	int getspp() {
		return spp;
	}
	const screenst &getscreen() {
		return screen;
	}
	const camerast &getcamera() {
		return camera;
	}
	const vector3f &getbackground() {
		return background;
	}
	std::vector<std::unique_ptr<primitive>> &getprimitives() {
		return primitives;
	}
	void load(const char *name) {
		struct {
			const char *name;
			void (scene_loader::*func)(const char *l);
		} router[] = {
			{"spp", &scene_loader::load_spp},
			{"render", &scene_loader::load_render},
			{"background", &scene_loader::load_background},
			{"screen", &scene_loader::load_screen},
			{"camera", &scene_loader::load_camera},
			{"texture", &scene_loader::load_texture},
			{"material", &scene_loader::load_material},
			{"mesh", &scene_loader::load_mesh},
			{"sphere", &scene_loader::load_sphere},
		};
		line = 0;
		textures.clear();
		materials.clear();
		primitives.clear();
		char buff[1024];
		FILE *fp = fopen(name, "rb");
		if (fp == nullptr) {
			fprintf(stderr, "scene load %s not exist\n", name);
			exit(-1);
		}
		while (!feof(fp)) {
			++line;
			type[0] = 0;
			fgets(buff, sizeof(buff), fp);
			sscanf(buff, "%s", type);
			if (type[0] == '#' || !isalpha(type[0]))
				continue ;
			for (auto &iter:router) {
				if (strcmp(iter.name, type) == 0) {
					auto f = iter.func;
					(this->*f)(buff);
				}
			}
		}
	}
};

