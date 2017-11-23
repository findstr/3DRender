#include <assert.h>
#include <stdint.h>
#include <gl/gl.h>
#include <glut/glut.h>

#include "graphic.h"

struct device {
	int width;
	int height;
	int bitdepth;
	int lockframe;
	uint8_t *framebuffer;
};

static struct device D;

static void
reshape(int width, int height) {

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //set the coordinate system, with the origin in the top left
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    return ;
}

static void
idle(void)
{
    glutPostRedisplay();
}


void
graphic_run(int width, int height, int bit_depth, void (*render)())
{
	int argc = 1;
	char *argv[] = {"demo"};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutCreateWindow("Rasterizer");

	D.width = width;
	D.height = height;
	D.bitdepth = bit_depth;
	D.lockframe = 0;
	D.framebuffer = (uint8_t *)malloc(width * height * (bit_depth / 8));

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
}

uint8_t *
graphic_lock_primary(uint8_t **buffer)
{
	assert(0);
	assert(buffer);
}

void
graphic_unlock_primary(uint8_t *buffer)
{
	assert(buffer);
	return ;
}

uint8_t *
graphic_lock_secondary(uint8_t **buffer, int *pitch)
{
	if (D.lockframe != 0)
		return NULL;
	D.lockframe = 1;
	return D.framebuffer;
}

void graphic_unlock_secondary(uint8_t *buffer)
{
	assert(buffer == D.framebuffer);
	assert(D.lockframe == 1);
	D.lockframe = 0;
}

void
graphic_fill_primary(int color)
{
    glClearColor(255, 255, 255, 0);
    glutSwapBuffers();
    return ;
}

void
graphic_fill_secondary(int color)
{
	int i = 0;
	int sz = D.width * D.height;
	uint8_t *buff = D.framebuffer;
	assert(D.lockframe == 1);
	while (i < sz) {
		switch (D.bitdepth) {
		case 3:
			*buff++ = (color >> 16) & 0xff;
		case 2:
			*buff++ = (color >> 8) & 0xff;
		case 1:
			*buff++ = (color >> 0) & 0xff;
		}
	}
}

void
graphic_flip_display()
{
	assert(D.lockframe == 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDrawPixels(D.width, D.height, GL_RGB, GL_UNSIGNED_BYTE, D.framebuffer);
	glEnable(GL_DEPTH_TEST);
	glutSwapBuffers();
	return ;
}


