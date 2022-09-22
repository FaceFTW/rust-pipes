
#include "../include/glad/glad.h"
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

void buildSphere() {
	GLint i, j;
	GLint stacks, slices;
	GLdouble radius;

	//TODO define slices macro
	slices = 16;
	stacks = slices;
	radius = 5;

	glTranslated(7, 7, 7);
	glPushMatrix();

	GLdouble thetaStep = 2 * PI / slices;
	GLdouble phiStep = PI / stacks;

	glBegin(GL_TRIANGLE_FAN);

	for(i = 0; i <= stacks; ++i) {
		GLdouble phiCurrent = (PI / 2) - i * phiStep;
		GLdouble rCosPhi = radius * cos(phiCurrent);
		GLdouble rSinPhi = radius * sin(phiCurrent);

		for(int j = 0; j <= slices; ++j) {
			GLdouble thetaCurrent = j * thetaStep;
			GLdouble xpos = rCosPhi * cos(thetaCurrent);
			GLdouble ypos = rCosPhi * sin(thetaCurrent);
			glNormal3d(xpos, ypos, rSinPhi);//Assumes GL_NORMALIZE
			glVertex3d(xpos, ypos, rSinPhi);
		}
	}

	glPopMatrix();
	glEnd();
}

void buildPipe(double length) {
	GLint stacks, slices;
	GLint j;
	GLdouble angle;
	GLdouble zLow, zHigh;
	GLdouble zNormal;
	GLdouble radius = 3.5;
	slices = 16;
	stacks = (int) std::round(((length / 7.0) * slices));

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	GLdouble angleStep = 2 * PI / slices;
	glMatrixMode(GL_MODELVIEW);

	//Rotate pipe before translate
	// glRotated(-90, 0, 1, 0);//for l/r align (X axis)
	// No rotation for fwd/backward align (z axis)?
	// glRotated(90, 0, 1, 0);//for f/b align (Z axis)?
	// glRotated(90, 1, 0, 0); //for vert align (Y axis)
	glTranslated(0, 0, 14);
	// glPushMatrix();

	glTranslated(7, 7, -14);
	// glPushMatrix();
	// glLoadIdentity();
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for(j = 0; j < stacks; j++) {
		zLow = j * length / stacks;
		zHigh = (j + 1) * length / stacks;

		for(angle = 0.0; angle <= 2 * PI + angleStep; angle += angleStep) {
			glNormal3d(sin(angle), cos(angle), zNormal);
			glEdgeFlag(true);
			glVertex3d(radius * sin(angle), radius * cos(angle), zLow);
			glVertex3d(radius * sin(angle), radius * cos(angle), zHigh);
		}
	}

	glEnd();

	//Pop transforms before
	// glPopMatrix();
	glPopMatrix();

	glFlush();
}

void draw(GLFWwindow* win) {
	glEnable(GL_NORMALIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50., 50., -50, 50., 0.5, 500);
	glPushMatrix();
	gluLookAt(-50, -50, 50, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);

	glBegin(GL_LINES);

	glColor3d(0.5, 0, 0.0);
	glVertex3d(-100, 0, 0);
	glVertex3d(0, 0, 0);
	glColor3d(1.0, 0, 0.0);
	glVertex3d(100, 0, 0);
	glVertex3d(0, 0, 0);
	glColor3d(0.0, 0.0, 0.5);
	glVertex3d(0, -100, 0);
	glVertex3d(0, 0, 0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 100, 0);
	glColor3d(0.0, 0.5, 0.0);
	glVertex3d(0, 0, -100);
	glVertex3d(0, 0, 0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 100);

	glEnd();

	//Draw rough bounding box
	glBegin(GL_LINE_LOOP);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3d(0, 0, 0);
	glVertex3d(14, 0, 0);
	glVertex3d(14, 0, 0);
	glVertex3d(14, 0, 14);
	glVertex3d(14, 14, 0);
	glVertex3d(0, 14, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 14);
	glVertex3d(14, 0, 0);
	glVertex3d(14, 14, 0);
	glVertex3d(14, 0, 14);
	glVertex3d(14, 14, 14);
	glVertex3d(0, 14, 14);
	glVertex3d(0, 0, 14);
	glVertex3d(0, 14, 14);
	glVertex3d(0, 14, 0);
	glEnd();

	//Now time to test the Game theory

	///ALONG Z AXIS (fwd/bkd)
	// glLoadIdentity();
	// glTranslatef(5, 5, 0);
	//glRotated(180, 0, 1, 0);
	// glPushMatrix();
	// buildPipe(7.0f);

	// glPopMatrix();
	// glLoadIdentity();
	// glTranslatef(5, 5, 7);
	//glRotated(180, 0, 1, 0);
	// buildPipe(7.0f);

	///ALONG Y AXIS (up/down)

	// buildPipe(14.0f);
	buildSphere();
	buildSphere();

	// buildPipe(14.0f);

	glLoadIdentity();
	glTranslated(14.0, 0, 0);
	glPushMatrix();
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
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) { exit(1); }
	glDrawBuffer(GL_FRONT_AND_BACK);

	draw(window);
	// gluCylinder();
	// NOTE: OpenGL error checks have been omitted for brevity
	while(!glfwWindowShouldClose(window)) { glfwPollEvents(); }

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
