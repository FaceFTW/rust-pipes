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

static void GlPipes::buildSphere() {
	GLint i, j;
	GLfloat sinCache1a[CACHE_SIZE];
	GLfloat cosCache1a[CACHE_SIZE];
	GLfloat sinCache2a[CACHE_SIZE];
	GLfloat cosCache2a[CACHE_SIZE];
	GLfloat sinCache1b[CACHE_SIZE];
	GLfloat cosCache1b[CACHE_SIZE];
	GLfloat sinCache2b[CACHE_SIZE];
	GLfloat cosCache2b[CACHE_SIZE];
	GLdouble angle;
	GLdouble zLow, zHigh;
	GLdouble sintemp1, sintemp2, sintemp3, sintemp4;
	GLdouble costemp3, costemp4;
	GLint start, finish;
	GLint stacks, slices;
	GLdouble radius;

	//TODO define slices macro
	slices = 16;
	stacks = slices;
	radius = 5.0;

	// invert sense of s - it seems the glu sphere is not built similarly
	// to the glu cylinder
	// (this probably means stacks don't grow along +z - check it out)

	/* Cache is the vertex locations cache */
	/* Cache2 is the various normals at the vertices themselves */

	// for(i = 0; i < slices; i++) {
	// 	angle = 2 * PI * i / slices;
	// 	sinCache1a[i] = (float) sin(angle);
	// 	cosCache1a[i] = (float) cos(angle);
	// 	sinCache2a[i] = sinCache1a[i];
	// 	cosCache2a[i] = cosCache1a[i];
	// }

	// for(j = 0; j <= stacks; j++) {
	// 	angle = PI * j / stacks;
	// 	sinCache2b[j] = (float) sin(angle);
	// 	cosCache2b[j] = (float) cos(angle);
	// 	sinCache1b[j] = radius * (float) sin(angle);
	// 	cosCache1b[j] = radius * (float) cos(angle);
	// }
	// /* Make sure it comes to a point */
	// sinCache1b[0] = 0.0f;
	// sinCache1b[stacks] = 0.0f;

	// sinCache1a[slices] = sinCache1a[0];
	// cosCache1a[slices] = cosCache1a[0];
	// sinCache2a[slices] = sinCache2a[0];
	// cosCache2a[slices] = cosCache2a[0];

	/* Do ends of sphere as TRIANGLE_FAN's (if not bTexture)
        ** We don't do it when bTexture because we need to respecify the
        ** texture coordinates of the apex for every adjacent vertex (because
        ** it isn't a constant for that point)
        */

	start = 1;
	finish = stacks - 1;

	GLdouble angleStep = 2 * PI / slices;

	/* Low end first (j == 0 iteration) */
	sintemp2 = sinCache1b[1];
	zHigh = cosCache1b[1];
	sintemp3 = sinCache2b[1];
	costemp3 = cosCache2b[1];
	glNormal3f(sinCache2a[0] * sinCache2b[0], cosCache2a[0] * sinCache2b[0], cosCache2b[0]);

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, radius);

	for(i = slices; i >= 0; i--) {
		glNormal3f(sinCache2a[i] * sintemp3, cosCache2a[i] * sintemp3, costemp3);
		glVertex3f(sintemp2 * sinCache1a[i], sintemp2 * cosCache1a[i], zHigh);
	}
	glEnd();

	/* High end next (j == stacks-1 iteration) */
	sintemp2 = sinCache1b[stacks - 1];
	zHigh = cosCache1b[stacks - 1];
	sintemp3 = sinCache2b[stacks - 1];
	costemp3 = cosCache2b[stacks - 1];
	glNormal3f(sinCache2a[stacks] * sinCache2b[stacks], cosCache2a[stacks] * sinCache2b[stacks],
	           cosCache2b[stacks]);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, -radius);
	for(i = 0; i <= slices; i++) {
		glNormal3f(sinCache2a[i] * sintemp3, cosCache2a[i] * sintemp3, costemp3);
		glVertex3f(sintemp2 * sinCache1a[i], sintemp2 * cosCache1a[i], zHigh);
	}
	glEnd();

	for(j = start; j < finish; j++) {
		zLow = cosCache1b[j];
		zHigh = cosCache1b[j + 1];
		sintemp1 = sinCache1b[j];
		sintemp2 = sinCache1b[j + 1];
		sintemp3 = sinCache2b[j + 1];
		costemp3 = cosCache2b[j + 1];
		sintemp4 = sinCache2b[j];
		costemp4 = cosCache2b[j];

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= slices; i++) {
			glNormal3f(sinCache2a[i] * sintemp3, cosCache2a[i] * sintemp3, costemp3);
			glVertex3f(sintemp2 * sinCache1a[i], sintemp2 * cosCache1a[i], zHigh);
			glNormal3f(sinCache2a[i] * sintemp4, cosCache2a[i] * sintemp4, costemp4);
			glVertex3f(sintemp1 * sinCache1a[i], sintemp1 * cosCache1a[i], zLow);
		}
		glEnd();
	}
}

static void GlPipes::buildElbow() {}