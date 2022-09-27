#include "include/nodes.h"
#include "include/glad/glad.h"
#include "include/utils.h"
#include <cmath>

using namespace GlPipes;

#define NODE_SCALE 14.0

Node::Node(Point* _pos, DrawConfig* _cfg) {
	pos = {_pos->x, _pos->y, _pos->z};
	cfg = _cfg;
}

PipeNode::PipeNode(Point* _pos, Axis _axis, DrawConfig* _cfg) : Node::Node(_pos, _cfg) {
	axis = _axis;
}

SphereNode::SphereNode(Point* _pos, DrawConfig* _cfg) : Node::Node(_pos, _cfg) {}

JointNode::JointNode(Point* _pos, Direction _start, Direction _end, DrawConfig* _cfg)
    : Node::Node(_pos, _cfg) {
	start = _start;
	end = _end;
}

void Node::draw() {}

void PipeNode::draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Apply Base Coordinate Transform (1 node = 14 u)
	glTranslated(14 * pos.x, 14 * pos.y, 14 * pos.z);
	glPushMatrix();

	//Apply bounding Box transform
	glLoadIdentity();
	switch(axis) {
		case AXIS_X: glRotated(-90, 0, 1, 0); break;
		case AXIS_Y: glRotated(90, 1, 0, 0); break;
		case AXIS_Z: glTranslated(0, 0, 14); break;
	}
	glTranslated(7, 7, -14);
	glPushMatrix();

	buildPipe(cfg);

	//Reset Stack
	glPopMatrix();
	glPopMatrix();
}

void SphereNode::draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Apply Base Coordinate Transform (1 node = 14 u)
	glTranslated(14 * pos.x, 14 * pos.y, 14 * pos.z);
	glPushMatrix();

	//Apply bounding Box transform
	glLoadIdentity();
	glTranslated(7, 7, 7);//Thank god its a uniform solid
	glPushMatrix();

	buildSphere(cfg);

	//Reset Stack
	glPopMatrix();
	glPopMatrix();
}

void JointNode::draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Apply Base Coordinate Transform (1 node = 14 u)
	glTranslated(14 * pos.x, 14 * pos.y, 14 * pos.z);
	glPushMatrix();

	buildElbow(cfg, start, end);

	glPopMatrix();
}
/********************************************************
DRAWING FUNCTIONS
*********************************************************/
static void GlPipes::buildPipe(DrawConfig* cfg) {
	GLdouble zLow, zHigh, zNormal = 0.0;
	int stacks = (int) std::round(((float) (cfg->pipeLength / 7.0) * cfg->pipeSlices));

	GLdouble angleStep = 2 * PI / cfg->pipeSlices;

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for(int i = 0; i < stacks; i++) {
		zLow = i * cfg->pipeLength / stacks;
		zHigh = (i + 1) * cfg->pipeLength / stacks;

		for(GLdouble angle = 0.0; angle <= 2 * PI + angleStep; angle += angleStep) {
			glNormal3f(sin(angle), cos(angle), zNormal);
			glEdgeFlag(true);
			glVertex3f(cfg->pipeRadius * sin(angle), cfg->pipeRadius * cos(angle), zLow);
			glVertex3f(cfg->pipeRadius * sin(angle), cfg->pipeRadius * cos(angle), zHigh);
		}
	}

	glEnd();
	glFlush();
}

//It's been a while since I had to do this much geometry
//Shoutouts to http://www.songho.ca/opengl/gl_sphere.html
static void GlPipes::buildSphere(DrawConfig* cfg) {
	GLdouble thetaStep = 2 * PI / cfg->sphereSlices;
	GLdouble phiStep = PI / cfg->sphereStacks;

	glBegin(GL_TRIANGLE_FAN);

	for(int i = 0; i <= cfg->sphereStacks; ++i) {
		GLdouble phiCurrent = (PI / 2) - i * phiStep;
		GLdouble rCosPhi = cfg->sphereRadius * cos(phiCurrent);
		GLdouble rSinPhi = cfg->sphereRadius * sin(phiCurrent);

		for(int j = 0; j <= cfg->sphereSlices; ++j) {
			GLdouble thetaCurrent = j * thetaStep;
			GLdouble xpos = rCosPhi * cos(thetaCurrent);
			GLdouble ypos = rCosPhi * sin(thetaCurrent);
			glNormal3d(xpos, ypos, rSinPhi);//Assumes GL_NORMALIZE
			glVertex3d(xpos, ypos, rSinPhi);
		}
	}
	glEnd();
}

//Assumes Base Transform has occured in JointNode::draw()
static void GlPipes::buildElbow(DrawConfig* cfg, Direction start, Direction end) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw Sphere
	glLoadIdentity();
	glTranslated(7, 7, 7);//Thank god its a uniform solid
	glPushMatrix();
	buildSphere(cfg);
	glPopMatrix();

	buildHalfPipe(cfg, start);
	buildHalfPipe(cfg, end);
}

static void GlPipes::buildHalfPipe(DrawConfig* cfg, Direction dir) {

	//Draw StartPipe
	glLoadIdentity();
	switch(dir) {
		case DIR_X_PLUS: glRotated(-90, 0, 1, 0); break;
		case DIR_X_MINUS:
			glRotated(-90, 0, 1, 0);
			glTranslated(0, 0, -7);
			break;
		case DIR_Y_PLUS: glRotated(90, 1, 0, 0); break;
		case DIR_Y_MINUS:
			glRotated(90, 1, 0, 0);
			glTranslated(0, 0, -7);
			break;
		case DIR_Z_PLUS: glTranslated(0, 0, 7); break;
		case DIR_Z_MINUS: glTranslated(0, 0, 14); break;

		case DIR_NONE: break;
	}
	glTranslated(7, 7, -7);
	glPushMatrix();

	//Halve pipe length temporarily
	cfg->pipeLength = cfg->pipeLength / 2;
	buildPipe(cfg);
	cfg->pipeLength = cfg->pipeLength * 2;

	//Reset Stack
	glPopMatrix();
}

#pragma region "VAO Code - Currently Unused"
/********************************************************
VERTEX FUNCTIONS
*********************************************************/
// static void generatePipeVertexArray(GLObjectData* obj, double length) {
// 	GLint stacks, slices;
// 	GLdouble angle;
// 	GLdouble zLow, zHigh;
// 	GLdouble zNormal;
// 	GLdouble radius = 5.0;
// 	slices = 16;
// 	stacks = (int) std::round(((float) (length / 7.0) * slices));

// 	zNormal = 0.0f;

// 	GLdouble angleStep = 2 * PI / slices;
// 	int angleStepCount = (2 * PI + angleStep) / angleStep;

// 	obj->vArraySize = 3 * stacks * angleStepCount * 2;
// 	obj->vArray = new GLdouble[obj->vArraySize];

// 	obj->nArraySize = 3 * stacks * angleStepCount;
// 	obj->nArray = new GLdouble[obj->nArraySize];

// 	for(int i = 0; i < stacks; i++) {
// 		zLow = i * length / stacks;
// 		zHigh = (i + 1) * length / stacks;

// 		for(int j = 0; j <= angleStepCount; j++) {
// 			GLdouble angle = j * angleStep;

// 			//Normals
// 			obj->nArray[(3 * (i * j))] = sin(angle);
// 			obj->nArray[(3 * (i * j)) + 1] = cos(angle);
// 			obj->nArray[(3 * (i * j)) + 2] = zNormal;

// 			//Vertices
// 			obj->vArray[(6 * (i * j))] = radius * sin(angle);
// 			obj->vArray[(6 * (i * j)) + 1] = radius * cos(angle);
// 			obj->vArray[(6 * (i * j)) + 2] = zLow;
// 			obj->vArray[(6 * (i * j)) + 3] = radius * sin(angle);
// 			obj->vArray[(6 * (i * j)) + 4] = radius * cos(angle);
// 			obj->vArray[(6 * (i * j)) + 5] = zHigh;
// 		}
// 	}
// }

// static void generateSphereVertexArray();
// static void generateElbowVertexArray();
#pragma endregion