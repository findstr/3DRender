#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef __APPLE__
#include <OPENGL/gl.h>
#else
#include <gl/gl.h>
#endif
#include <glut/glut.h>

#include "driver.h"

static char keyboard[0x100];

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

static void
keydown(unsigned char key, int x, int y)
{
	int mod;
	if (key > 255)
		return ;
	int val = DRIVER_KEY_NORMAL;
	mod = glutGetModifiers();
	if (mod & GLUT_ACTIVE_CTRL)
		val |= DRIVER_KEY_CTRL;
	if (mod & GLUT_ACTIVE_ALT)
		val |= DRIVER_KEY_ALT;
	if (mod & GLUT_ACTIVE_SHIFT)
		val |= DRIVER_KEY_SHIFT;
	keyboard[key] = val;
	printf("key Down:%x\n", val);
}

static void
keyup(unsigned char key, int x, int y)
{
	if (key > 255)
		return ;
	keyboard[key] = 0;
	printf("key UP:%x\n", key);
}


void
driver_run(int width, int height, void (*render)())
{
	int argc = 1;
	char *argv[] = {"demo"};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutCreateWindow("Rasterizer");

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keydown);
	glutKeyboardUpFunc(keyup);
	glutMainLoop();
}

void
driver_draw(const uint8_t *frame, size_t width, size_t height)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, frame);
	glEnable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

int
driver_keydown(char k, int ctrl)
{
	return (keyboard[k] & ctrl) ? 1 : 0;
}


