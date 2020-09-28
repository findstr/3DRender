#pragma once
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene.h"

class scene_loader {
private:
	int line = 0;
	char type[64];
	scene *s;
	std::vector<std::shared_ptr<texture>> textures;
	std::vector<std::shared_ptr<material>> materials;
	std::vector<std::shared_ptr<primitive>> primitives;
private:
	typedef void (*load_t)(const char *line);
	void load_texture(const char *line) {
		char path[PATH_MAX];
		sscanf(line, "%s %s", type, path);
		textures.emplace_back(new texture(path));
	}
	void load_material(const char *buff) {
		char subtype[64];
		enum material::type typ;
		unsigned int textureid;
		vector3f albedo;
		float roughness_or_kd;
		float metallic_or_ks;
		float ior = 1.f;
		std::shared_ptr<texture> tex(nullptr);
		sscanf(buff, "%s %s %d %f,%f,%f %f %f %f",
			type, subtype, &textureid,
			&albedo.x(), &albedo.y(), &albedo.z(),
			&roughness_or_kd, &metallic_or_ks, &ior);
		if (textureid >= 0) {
			if (textures.size() <= textureid) {
				fprintf(stderr, "line:%d incorrect textureid\n", line);
				exit(-1);
			}
			tex = textures[textureid];
		}
		if (strcmp(subtype, "microfacet") == 0) {
			typ = material::MICROFACET;
		} else if (strcmp(subtype, "diffuse") == 0) {
			typ = material::DIFFUSE;
		} else if (strcmp(subtype, "light") == 0) {
			typ = material::LIGHT;
		} else if (strcmp(subtype, "glass") == 0) {
			typ = material::GLASS;
		} else {
			fprintf(stderr, "line:%d incorrect material type:%s\n", line, subtype);
			exit(0);
		}
		materials.emplace_back(new material(typ, tex,
			albedo, roughness_or_kd, metallic_or_ks, ior));
	}
	void load_mesh(const char *buff) {
		char path[PATH_MAX];
		unsigned int materialid;
		sscanf(buff, "%s %s %d", type, path, &materialid);
		if (materialid >= materials.size()) {
			fprintf(stderr, "line:%d incorrect material id:%d\n", line, materialid);
			exit(0);
		}
		std::unique_ptr<primitive> obj(new mesh(path, materials[materialid]));
		s->add(obj);
	}
	void load_sphere(const char *buff) {
		vector3f center;
		float radius;
		unsigned int materialid;
		sscanf(buff, "%s %f,%f,%f %f %d", type,
			&center.x(), &center.y(), &center.z(),
			&radius, &materialid);
		if (materialid >= materials.size()) {
			fprintf(stderr, "line:%d incorrect material id:%d\n", line, materialid);
			exit(0);
		}
		std::unique_ptr<primitive> obj(new sphere(center, radius, materials[materialid]));
		s->add(obj);
	}
public:
	void load(scene &s, const char *name) {
		struct {
			const char *name;
			void (scene_loader::*func)(const char *l);
		} router[] = {
			{"texture", &scene_loader::load_texture},
			{"material", &scene_loader::load_material},
			{"mesh", &scene_loader::load_mesh},
			{"sphere", &scene_loader::load_sphere},
		};
		this->s = &s;
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

