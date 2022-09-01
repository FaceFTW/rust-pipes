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
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#define CACHE_SIZE 100

#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define TEAPOT_SIZE 2.0
#define PI 3.14159265358979323846f

//Perspective Constants
#define FOV 90
#define ASPECT_RATIO 1.0
#define Z_NEAR 1
#define Z_FAR 1000

typedef struct strRGBA {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} RGBA;

typedef struct _MATERIAL {
	RGBA ka;
	RGBA kd;
	RGBA ks;
	GLfloat specExp;
} MATERIAL;

void buildPipe(float length, float s_start, float s_end) {
	GLint stacks, slices;
	GLint i, j;
	GLfloat sinCache[CACHE_SIZE];
	GLfloat cosCache[CACHE_SIZE];
	GLfloat sinCache2[CACHE_SIZE];
	GLfloat cosCache2[CACHE_SIZE];
	GLfloat angle;
	GLfloat zLow, zHigh;
	GLfloat zNormal;
	GLfloat s_delta;
	GLfloat radius = 5.0f;
	slices = 16;
	stacks = (int) std::round(((float) (length / 7.0f) * slices));

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	s_delta = s_end - s_start;

	for(i = 0; i < slices; i++) {
		angle = 2 * PI * i / slices;
		sinCache2[i] = (float) sin(angle);
		cosCache2[i] = (float) cos(angle);
		sinCache[i] = (float) sin(angle);
		cosCache[i] = (float) cos(angle);
	}

	sinCache[slices] = sinCache[0];
	cosCache[slices] = cosCache[0];
	sinCache2[slices] = sinCache2[0];
	cosCache2[slices] = cosCache2[0];

	int listNum = glGenLists(1);

	glNewList(listNum, GL_COMPILE_AND_EXECUTE);

	for(j = 0; j < stacks; j++) {
		zLow = j * length / stacks;
		zHigh = (j + 1) * length / stacks;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= slices; i++) {
			glNormal3f(sinCache2[i], cosCache2[i], zNormal);
			glVertex3f(radius * sinCache[i], radius * cosCache[i], zLow);
			glVertex3f(radius * sinCache[i], radius * cosCache[i], zHigh);
		}
		glEnd();
	}

	glEndList();
	glCallList(listNum);
}

void draw() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 40.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glColor3f(1.0, 0.0, 0.0);

	// glutWireTeapot(0.1);

	// glFlush();
	// glColor3f(0.0, 1.0, 0.0);

	MATERIAL pMat;

	pMat.ka.r = 0.0215f;
	pMat.ka.g = 0.1745f;
	pMat.ka.b = 0.0215f;
	pMat.ka.a = 0.5f;
	pMat.kd.r = 0.07568f;
	pMat.kd.g = 0.61424f;
	pMat.kd.b = 0.07568f;
	pMat.kd.a = 0.5f;
	pMat.ks.r = 0.633f;
	pMat.ks.g = 0.727811f;
	pMat.ks.b = 0.633f;
	pMat.specExp = 0.6f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*) &pMat.ka);
	glMaterialfv(GL_BACK, GL_AMBIENT, (GLfloat*) &pMat.ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*) &pMat.kd);
	glMaterialfv(GL_BACK, GL_DIFFUSE, (GLfloat*) &pMat.kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*) &pMat.ks);
	glMaterialfv(GL_BACK, GL_SPECULAR, (GLfloat*) &pMat.ks);
	glMaterialf(GL_FRONT, GL_SHININESS, pMat.specExp * 128.0f);
	glMaterialf(GL_BACK, GL_SHININESS, pMat.specExp * 128.0f);

	buildPipe(100.0f, 0.0f, 0.0f);

	glFlush();
}

void keypressCB(unsigned char key, int x, int y) {
	//do anything to exit
	if(key) exit(0);
}

int main(int argc, char* argv[]) {
	int win;//A window pointer?

	// glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_RGB);
	// glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	// win = glutCreateWindow("teapot :)");

	// glEnable(GL_DEPTH_TEST);
	// glClearColor(0.0, 0.0, 0.0, 0.0);
	// gluPerspective(FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

	// glutDisplayFunc(draw);
	// glutKeyboardFunc(keypressCB);

	// glutMainLoop();

	return 0;
}