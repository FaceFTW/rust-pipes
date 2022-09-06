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
}

/********************************************************
DRAWING FUNCTIONS
*********************************************************/
static void buildPipe(float length) {
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

static void buildSphere() {}

static void buildElbow() {}