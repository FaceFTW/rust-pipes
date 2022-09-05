/**
 * @file scratch/main.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief A test program to check if I understand how to *not* use X11
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
#include "../include/glad/gl.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
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

void buildPipe(float length) {
	GLint stacks, slices;
	GLint j;
	GLfloat angle;
	GLfloat zLow, zHigh;
	GLfloat zNormal;
	GLfloat radius = 5.0f;
	slices = 16;
	stacks = (int) std::round(((float) (length / 7.0f) * slices));

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	GLfloat angleStep = 2 * PI / slices;

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	for(j = 0; j < stacks; j++) {
		zLow = j * length / stacks;
		zHigh = (j + 1) * length / stacks;

		for(angle = 0.0; angle <= 2 * PI + angleStep; angle += angleStep) {
			glNormal3f(sin(angle), cos(angle), zNormal);
			glEdgeFlag(true);
			glVertex3f(radius * sin(angle), radius * cos(angle), zLow);
			glVertex3f(radius * sin(angle), radius * cos(angle), zHigh);
		}
	}

	glEnd();
	glFlush();
}

void draw(GLFWwindow* win) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100., 100., -100, 100., 0.5, 500);
	glPushMatrix();
	gluLookAt(-100, 100, -100, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);

	// MATERIAL pMat;

	// pMat.ka.r = 0.0215f;
	// pMat.ka.g = 0.1745f;
	// pMat.ka.b = 0.0215f;
	// pMat.ka.a = 0.5f;
	// pMat.kd.r = 0.07568f;
	// pMat.kd.g = 0.61424f;
	// pMat.kd.b = 0.07568f;
	// pMat.kd.a = 0.5f;
	// pMat.ks.r = 0.633f;
	// pMat.ks.g = 0.727811f;
	// pMat.ks.b = 0.633f;
	// pMat.specExp = 0.6f;

	// glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*) &pMat.ka);
	// glMaterialfv(GL_BACK, GL_AMBIENT, (GLfloat*) &pMat.ka);
	// glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*) &pMat.kd);
	// glMaterialfv(GL_BACK, GL_DIFFUSE, (GLfloat*) &pMat.kd);
	// glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*) &pMat.ks);
	// glMaterialfv(GL_BACK, GL_SPECULAR, (GLfloat*) &pMat.ks);
	// glMaterialf(GL_FRONT, GL_SHININESS, pMat.specExp * 128.0f);
	// glMaterialf(GL_BACK, GL_SHININESS, pMat.specExp * 128.0f);

	glBegin(GL_LINES);

	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(-100, 0, 0);
	glVertex3f(100, 0, 0);
	glVertex3f(0, -100, 0);
	glVertex3f(0, 100, 0);
	glVertex3f(0, 0, -100);
	glVertex3f(0, 0, 100);

	glEnd();

	glLoadIdentity();
	glRotatef(270, 0, 0, 0.1);
	glTranslatef(10, 1, 1);
	buildPipe(100.0f);

	glLoadIdentity();
	glRotatef(90, 1, 0, 0);
	glTranslatef(-10, -10, -10);
	buildPipe(100.0f);
	glfwSwapBuffers(win);
}

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;

	glfwSetErrorCallback(error_callback);

	if(!glfwInit()) exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(600, 600, "Simple example", NULL, NULL);
	if(!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glDrawBuffer(GL_FRONT_AND_BACK);

	draw(window);
	// gluCylinder();
	// NOTE: OpenGL error checks have been omitted for brevity
	while(!glfwWindowShouldClose(window)) { glfwPollEvents(); }

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
