#include "include/glad/glad.h"
#include "include/pipe_layer.h"
#include <cmath>

using namespace GlPipes;

PipeNode::PipeNode(Point* _pos, Axis _axis) {
	pos = _pos;
	axis = _axis;
}

SphereNode::SphereNode(Point* _pos) { pos = _pos; }

JointNode::JointNode(Point* _pos, Direction _start, Direction _end) {
	pos = _pos;
	start = _start;
	end = _end;
}

void PipeNode::draw() {
	//Apply Transforms based on node position
	//Pipe Length represents a "bounding box of where the pipe can be placed"
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//change used matrix based on axis
	switch(axis) {
		case AXIS_X:
			glTranslated(0, 0, 0);
			glRotated(0, 0, 0, 0);
			break;
		case AXIS_Y:
			glTranslated(0, 0, 0);
			glRotated(0, 0, 0, 0);
			break;
		case AXIS_Z:
			glTranslated(0, 0, 0);
			glRotated(0, 0, 0, 0);
			break;
	}
	//TODO ad pipelength macro
	buildPipe(14.0);
}

/********************************************************
DRAWING FUNCTIONS
*********************************************************/
static void GlPipes::buildPipe(double length) {
	GLint stacks, slices;
	GLint j;
	GLdouble angle;
	GLdouble zLow, zHigh;
	GLdouble zNormal;
	GLdouble radius = 5.0;
	slices = 16;
	stacks = (int) std::round(((float) (length / 7.0) * slices));

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	GLdouble angleStep = 2 * PI / slices;

	glColor3f(1.0, 1.0, 1.0);
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

//It's been a while since I had to do this much geometry
//Shoutouts to http://www.songho.ca/opengl/gl_sphere.html
static void GlPipes::buildSphere() {
	GLint i, j;
	GLint stacks, slices;
	GLdouble radius;

	//TODO define slices macro
	slices = 16;
	stacks = slices;
	radius = 5.0;

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
	glEnd();
}

static void GlPipes::buildElbow() {}