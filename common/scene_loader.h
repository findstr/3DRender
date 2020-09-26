#pragma once
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene.h"

//TODO: refine to class
static inline void
scene_load(scene &s, const char *name)
{
	FILE *fp = fopen(name, "rb");
	if (fp == nullptr) {
		fprintf(stderr, "scene load %s not exist\n", name);
		exit(-1);
	}
	int line = 0;
	char buff[1024];
	std::vector<std::shared_ptr<texture>> textures;
	std::vector<std::shared_ptr<material>> materials;
	std::vector<std::shared_ptr<primitive>> primitives;
	while (!feof(fp)) {
		char type[1024];
		++line;
		fgets(buff, sizeof(buff), fp);
		sscanf(buff, "%s", type);
		if (type[0] == '#')
			continue ;
		if (strcmp(type, "texture") == 0) {
			char path[PATH_MAX];
			sscanf(buff, "%s %s", type, path);
			textures.emplace_back(new texture(path));
		} else if (strcmp(type, "material") == 0) {
			char subtype[64];
			enum material::type typ;
			int textureid;
			vector3f albedo;
			float roughness_or_kd;
			float metallic_or_ks;
			std::shared_ptr<texture> tex(nullptr);
			sscanf(buff, "%s %s %d %f,%f,%f %f %f",
				type, subtype, &textureid,
				&albedo.x(), &albedo.y(), &albedo.z(),
				&roughness_or_kd, &metallic_or_ks);
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
			} else {
				fprintf(stderr, "line:%d incorrect material type:%s\n", line, subtype);
				exit(0);
			}
			materials.emplace_back(new material(
				typ, tex, albedo, roughness_or_kd, metallic_or_ks));
		} else if (strcmp(type, "mesh") == 0) {
			char path[PATH_MAX];
			unsigned int materialid;
			sscanf(buff, "%s %s %d", type, path, &materialid);
			if (materialid >= materials.size()) {
				fprintf(stderr, "line:%d incorrect material id:%d\n", line, materialid);
				exit(0);
			}
			std::unique_ptr<primitive> obj(new mesh(path, materials[materialid]));
			s.add(obj);
		} else if (strcmp(type, "sphere") == 0) {
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
		}
	}
	return ;
}

