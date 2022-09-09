/**
 * @file node_struct.h
 * @author Alex "FaceFTW" Westerman
 * @brief A new and improved structure
 * @version 0.1
 * @date 2022-06-27
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#ifndef __NODE_STRUCT_H_
#define __NODE_STRUCT_H_

#include "utils.h"
#include <vector>

namespace GlPipes {

#pragma region "Node Defines"

class Node {
public:
	void draw();

protected:
	Point* pos;
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

class JointNode: public Node {
public:
	JointNode(Point* _pos, Direction _start, Direction _end);
	void draw();

protected:
	Direction start;
	Direction end;
};

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

//A wrapper for a 2-depth vector of points
class PipeList {
public:
	PipeList(int pipeCnt);
	~PipeList();

	std::vector<Point*>*& operator[](int pipe);
	Point*& operator()(int pipe, int idx);

	void addToPipe(int pipe, Point* point);

private:
	std::vector<std::vector<Point*>*>* internal;
};

class PipeLayer {
public:
	PipeLayer(Point* node_size, int numPipes);
	~PipeLayer();// Destructor

	std::vector<Point*>*& operator()(int pipeIdx);
	Node*& operator[](Point* pos);

	Point* size();
	uint size(Axis d);
	bool isEmpty(Point* pos);

	void generatePipe(int pipeIdx);

	Point** getNeighbors(Point* pos);
	Direction chooseRandomDirection(Point* pos, Direction dir, int weight);
	Direction chooseRandomInitialDirection(Point* pos);
	Direction chooseRandomEmptyDirection(Point* pos);

	/**
	 * @brief Search for an empty node to start drawing
	 * - Marks node as taken (mf: renam fn to ChooseEmptyNode ?
	 *
	 * @param ip3dEmpty Return position of empty node
	 * @return true
	 * @return false if couldn't find a node
	 */
	Point* findRandomEmptyNode();

	/**
	 * @brief Get position of next node from curPos and lastDir
	 *
	 * @param curPos Pointer to origin node position
	 * @param dir Direction to find the next node
	 * @return Pointer to next node position
	 */
	Point* getNextNodePos(Point* curPos, Direction dir);

	//TODO evaluate if Needed
	/**
	 * @brief Get list of direction indices of empty node neighbours, and put them in supplied matrix
	 *
	 * @param FnNode
	 * @param nEmpty
	 * @return int number of empty node neighbours
	 */
	int getEmptyNeighbors(Point* pos, Direction* emptyDirs);

	int countAvailableInDirection(Point* pos, Direction dir);

private:
	Node**** node_struct;// For a 3-dimensional array (better as pointers imho)
	PipeList* pipes;
	Point* node_struct_size;
};

}// namespace GlPipes

#endif