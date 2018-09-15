#ifndef _PIPELINE_H
#define _PIPELINE_H

struct camera;
struct object;

void pipeline_add_camera(struct camera *cam);
void pipeline_add_object(struct object *obj);
void pipeline_start(int width, int height, void (*update)());
void pipeline_run();

#endif

