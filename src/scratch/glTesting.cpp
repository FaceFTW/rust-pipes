#include "../include/glad/glad.h"
#include "../include/pipe_layer.h"
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

void drawAxes() {
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
}

void drawBoundingBox() {

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
}

void draw(GLFWwindow* win) {
	glEnable(GL_NORMALIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50., 50., -50, 50., 0.5, 500);
	glPushMatrix();
	gluLookAt(-50, -50, -50, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	drawAxes();
	drawBoundingBox();

	DrawConfig cfg = {14, 16, 3, 16, 16, 5};
	Point testPoint = {0, 0, 0};

	GlPipes::JointNode testJoint =
	        GlPipes::JointNode(&testPoint, Direction::DIR_X_PLUS, Direction::DIR_Y_MINUS, &cfg);
	testJoint.draw();

	testPoint.x = 1;
	testJoint = GlPipes::JointNode(&testPoint, Direction::DIR_X_PLUS, Direction::DIR_Z_PLUS, &cfg);
	testJoint.draw();
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
