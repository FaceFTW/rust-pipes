#ifndef __PIPELAYER_H_
#define __PIPELAYER_H_

#include "nodes.h"
#include "utils.h"
#include <array>
#include <vector>

namespace GlPipes {

//A wrapper for a 2-depth vector of points
class PipeList {
public:
	PipeList(int pipeCnt);
	~PipeList();

	std::vector<Point>& operator[](int pipe);
	Point& operator()(int pipe, int idx);

	void addToPipe(int pipe, Point* point);
	void outputPipePath(int pipe);

private:
	std::vector<Point>* internal;
	int pipeCnt;
};

class PipeLayer {
public:
	PipeLayer(Point node_size, int numPipes);
	~PipeLayer();// Destructor

	std::vector<Point>& operator()(int pipeIdx);
	Node*& operator[](Point* pos);

	Point* size();
	unsigned int size(Axis d);
	bool isEmpty(Point* pos);

	void generatePipe(int pipeIdx, DrawConfig* cfg);
	void outputPipePath(int pipeIdx);
	void drawPipe(int pipeIdx);

	int getEmptyNeighbors(Point pos, Direction* emptyDirs);
	Point findRandomEmptyNode();
	Point getNextNodePos(Point curPos, Direction dir);
	Point* getNeighbors(Point pos);
	int countAvailableInDirection(Point pos, Direction dir);
	Direction chooseRandomDirection(Point pos);

private:
	Node**** node_struct;
	PipeList* pipes;
	Point node_struct_size;
};

}// namespace GlPipes

#endif
