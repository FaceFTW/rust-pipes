/**
 * @file pipe_test_nox11.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief A test program to check if I understand how to *not* use X11
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define TEAPOT_SIZE 2.0

//Perspective Constants
#define FOV 90
#define ASPECT_RATIO 1.0
#define Z_NEAR 1
#define Z_FAR 1000

void draw() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glColor3f(1.0, 0.0, 0.0);
	glutWireTeapot(0.5);
	glFlush();
}

void keypressCB(unsigned char key, int x, int y) {
	//do anything to exit
	if(key) exit(0);
}

int main(int argc, char* argv[]) {
	int win;//A window pointer?

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	win = glutCreateWindow("teapot :)");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	gluPerspective(FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

	glutDisplayFunc(draw);
	glutKeyboardFunc(keypressCB);

	glutMainLoop();

	return 0;
}