#ifndef __NODES_H_
#define __NODES_H_

#include "./utils.h"
namespace GlPipes {

class Node {
public:
	virtual void draw() = 0;
	Node(Point* _pos, DrawConfig* _cfg);

protected:
	Point pos;
	DrawConfig* cfg;
};

class PipeNode: public Node {
public:
	PipeNode(Point* _pos, Axis _axis, DrawConfig* _cfg);
	void draw();

protected:
	Axis axis;
};

class SphereNode: public Node {
public:
	SphereNode(Point* _pos, DrawConfig* _cfg);
	void draw();
};

class JointNode: public Node {
public:
	JointNode(Point* _pos, Direction _start, Direction _end, DrawConfig* _cfg);
	void draw();

protected:
	Direction start;
	Direction end;
};

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
// static void generatePipeVertexArray(GLObjectData* obj, double length);
// static void generateSphereVertexArray();
// static void generateElbowVertexArray();
;
/********************************************************
DRAWING FUNCTIONS
*********************************************************/
static void buildPipe(DrawConfig* cfg);
static void buildSphere(DrawConfig* cfg);
static void buildElbow(DrawConfig* cfg, Direction start, Direction end);
static void buildHalfPipe(DrawConfig* cfg, Direction dir);
;

/********************************************************
PIPE DRAWING TRANSFORMS
*********************************************************/

}// namespace GlPipes
#endif
