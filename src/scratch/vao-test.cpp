
#include "../include/glad/glad.h"
#include "../include/nodes.h"
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

void draw(GLFWwindow* win) {
	glEnable(GL_NORMALIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100., 100., -100, 100., 0.5, 500);
	glPushMatrix();
	gluLookAt(-100, 100, -100, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);

	glBegin(GL_LINES);

	glColor3d(1.0, 0, 0.0);
	glVertex3d(-100, 0, 0);
	glVertex3d(100, 0, 0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, -100, 0);
	glVertex3d(0, 100, 0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, -100);
	glVertex3d(0, 0, 100);
	glEnd();

	GLObjectData* data = new GLObjectData();

	GlPipes::generatePipeVertexArray(data, 14.0);


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
