#include <unistd.h>
#include "screen.h"
#include "texture.h"
#include "material.h"
#include "primitive.h"
#include "scene_loader.h"
#include "pathtracer.h"
#include "raytracer.h"

int main()
{
	scene scene1;
	screen scrn(800, 800);
	pathtracer pr;
	raytracer rr;
	scene_loader sl;
	sl.load(scene1, "scene/whitted.scene");
	scrn.clear();
	rr.render(scene1, scrn);
	scrn.dump("out.ppm");
	return 0;
}

