#ifndef __NODES_H_
#define __NODES_H_

#include "./utils.h"
namespace GlPipes {

class Node {
public:
	virtual void draw() = 0;
	Node(Point* _pos);
protected:
	Point pos;
};

class PipeNode: public Node {
public:
	PipeNode(Point* _pos, Axis _axis);
	void draw();

protected:
	Axis axis;
};

class SphereNode: public Node {
public:
	SphereNode(Point* _pos);
	void draw();
};

// class JointNode: public Node {
// public:
// 	JointNode(Point* _pos, Direction _start, Direction _end);
// 	void draw();

// protected:
// 	Direction start;
// 	Direction end;
// };

// class CapNode: public Node {
// public:
// 	CapNode(Point* _post, Direction _ext);
// 	void draw();

// protected:
// 	Direction ext;//Defines direction of pipe out of cap
// };

#pragma endregion

/********************************************************
VERTEX FUNCTIONS
*********************************************************/
static void generatePipeVertexArray(GLObjectData* obj, double length);
static void generateSphereVertexArray();
static void generateElbowVertexArray();
;
/********************************************************
DRAWING FUNCTIONS
*********************************************************/
static void buildPipe(double length);
static void buildSphere();
static void buildElbow();

}// namespace GlPipes
#endif
