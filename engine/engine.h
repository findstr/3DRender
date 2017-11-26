#ifndef _ENGINE_H
#define _ENGINE_H

struct camera;
struct object;

void engine_add_camera(struct camera *cam);
void engine_add_object(struct object *obj);
void engine_start(int width, int height, void (*update)());
void engine_run();

#endif

