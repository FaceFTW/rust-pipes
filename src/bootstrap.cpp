/**
 * @file bootstrap.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief Main Entry Point for the application
 * This is mostly written by myself
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "include/global_state.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

//Generic Error Callback func
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char* argv[]) {
	GLFWwindow* window;

	//For teapot drawing
	//TODO figure out best way to get *only* that code
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_RGB);
	glfwSetErrorCallback(error_callback);

	//GLFW Init
	if(!glfwInit()) {
		//Init falied, terminate with code 1
		exit(EXIT_FAILURE);
	}

	//Use OpenGL 2, the original code based on 1.1 *should*
	//be automatically compatible
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//TODO Add fullscreen/novisible flags, go to Frame buffer

	//Start the GL window
	window = glfwCreateWindow(640, 480, "gl-pipes", NULL, NULL);
	if(!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	//Loop Configuration

	glfwSwapInterval(1);

	//Make the state before making the window
	State global;

	// glEnable(GL_DEPTH_TEST);
	// glClearColor(0.0, 0.0, 0.0, 0.0);
	// gluPerspective(FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

	// glutDisplayFunc();
	// glutKeyboardFunc(keypressCB);

	//setup the refs it needs

	// extern void (State::*draw)();
	// typedef void (*fptr)(State *);

	// fptr p = (fptr) (global.*draw);

	// void (State::*idleCallback)(void*) = &State::draw;
	// glutIdleFunc((&global)->*idleCallback);

	// glutMainLoop();

	//Main Loop
	while(!glfwWindowShouldClose(window)) {
		global.draw();

		glfwSwapBuffers(window);
		//glfwPollEvents();
		glfwWaitEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
	;
}