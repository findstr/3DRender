#ifndef _MATRIAL_H
#define _MATRIAL_H

#include "rgb.h"
#include "texture.h"
#include "shader.h"

struct matrial {
	char name[64];
	rgba_t color;
	float ka, kd, ks, power;
	rgba_t ra, rd, rs;
	struct texture texture;
	struct shader_routine shader;
	struct matrial *next;
};


struct matrial *matrial_create();



#endif

