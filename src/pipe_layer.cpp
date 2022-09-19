#include "include/pipe_layer.h"
#include "include/nodes.h"
#include "include/utils.h"
#include <cstddef>

using namespace GlPipes;

/******CONSTRUCTORS/DESTRUCTORS*******/

PipeLayer::PipeLayer(Point node_size, int numPipes) {
	node_struct_size = node_size;
	//TODO macro or dynamic pipe count
	pipes = new PipeList(numPipes);

	node_struct = new Node***[node_struct_size.x];
	for(int i = 0; i < node_struct_size.x; i++) {
		node_struct[i] = new Node**[node_struct_size.y];
		for(int j = 0; j < node_struct_size.y; j++) {
			node_struct[i][j] = new Node*[node_struct_size.z];
		}
	}
}

PipeLayer::~PipeLayer() {
	for(int i = 0; i < node_struct_size.x; i++) {
		for(int j = 0; j < node_struct_size.y; j++) { delete[] node_struct[i][j]; }
		delete[] node_struct[i];
	}
	delete[] node_struct;
	delete pipes;
}
/******OVERLOADED OPERATORS*******/
std::vector<Point>& PipeLayer::operator()(int pipeIdx) { return (*pipes)[pipeIdx]; }

Node*& PipeLayer::operator[](Point* pos) { return node_struct[pos->x][pos->y][pos->z]; }

void PipeLayer::generatePipe(int pipeIdx) {
	Point startPos = findRandomEmptyNode();
	pipes->addToPipe(pipeIdx, &startPos);

	//Choose a random starting direction that is empty
	Direction nextDir = chooseRandomDirection(startPos);
	if(nextDir == DIR_NONE) {
		return;//Can't do sh*t
	}

	node_struct[startPos.x][startPos.y][startPos.z] =
	        new PipeNode(&startPos, getAxisFromDirection(nextDir));

	//Choose a random number of iterations (minimum 5 to maximum 10 for now)
	int numIter = iRand2(5, 10);

	for(int i = 0; i < numIter; i++) {
		//Determine how many nodes are available in that direction
		int openNodesInDir = countAvailableInDirection(startPos, nextDir);

		//Choose a random number n, 1 <= n <= available nodes
		int pipeLength = iRand2(1, openNodesInDir);

		//For n-1 available nodes, create straight pipe segs in dir
		//Logically, if only one node is available, this will not run
		for(int j = 0; j < pipeLength - 1; j++) {
			startPos = getNextNodePos(startPos, nextDir);
			pipes->addToPipe(pipeIdx, &startPos);
			node_struct[startPos.x][startPos.y][startPos.z] =
			        new PipeNode(&startPos, getAxisFromDirection(nextDir));
		}

		//For the nth node (is not a loop)
		// new SphereNode(startPos);
		startPos = getNextNodePos(startPos, nextDir);

		//choose a random empty direction
		Direction currentDir = nextDir;
		nextDir = chooseRandomDirection(startPos);

		//Add a pipe joint that turns to that direction
		//TODO change to jointnode when ready
		pipes->addToPipe(pipeIdx, &startPos);
		node_struct[startPos.x][startPos.y][startPos.z] =
		        new PipeNode(&startPos, getAxisFromDirection(currentDir));
	}

	//Pop the last node, replace it with a Pipe Ending Node
	new SphereNode(&startPos);
	//Profit
}

Point* PipeLayer::size() { return &node_struct_size; }

uint PipeLayer::size(Axis d) {
	switch(d) {

		case AXIS_X: return node_struct_size.x;
		case AXIS_Y: return node_struct_size.y;
		case AXIS_Z: return node_struct_size.z;
	}
}

bool PipeLayer::isEmpty(Point* pos) { return node_struct[pos->x][pos->y][pos->z] != NULL; }

Point PipeLayer::getNextNodePos(Point curPos, Direction dir) {
	switch(dir) {
		case DIR_X_PLUS:
			if((curPos.x) + 1 < this->size(AXIS_X)) return Point(&curPos, DIR_X_PLUS);
			break;
		case DIR_X_MINUS:
			if((curPos.x) - 1 < 0) return Point(&curPos, DIR_X_MINUS);
			break;
		case DIR_Y_PLUS:
			if((curPos.y) + 1 < this->size(AXIS_Y)) return Point(&curPos, DIR_Y_PLUS);
			break;
		case DIR_Y_MINUS:
			if((curPos.y) - 1 < 0) return Point(&curPos, DIR_Y_MINUS);
			break;
		case DIR_Z_PLUS:
			if((curPos.z) + 1 < this->size(AXIS_Z)) return Point(&curPos, DIR_Z_PLUS);
			break;
		case DIR_Z_MINUS:
			if((curPos.z) - 1 < 0) return Point(&curPos, DIR_Z_MINUS);
			break;
		case DIR_NONE: break;
	}
	return curPos;
}

Point PipeLayer::findRandomEmptyNode() {
	Point pos = {0, 0, 0};

	do {
		pos.x = iRand(node_struct_size.x);
		pos.y = iRand(node_struct_size.y);
		pos.z = iRand(node_struct_size.z);
	} while(!isEmpty(&pos));

	return pos;
}

Point* PipeLayer::getNeighbors(Point pos) {
	Point* neighbors = new Point[6];
	neighbors[DIR_X_PLUS] = getNextNodePos(pos, DIR_X_PLUS);
	neighbors[DIR_X_MINUS] = getNextNodePos(pos, DIR_X_MINUS);
	neighbors[DIR_Y_PLUS] = getNextNodePos(pos, DIR_Y_PLUS);
	neighbors[DIR_Y_MINUS] = getNextNodePos(pos, DIR_Y_MINUS);
	neighbors[DIR_Z_PLUS] = getNextNodePos(pos, DIR_Z_PLUS);
	neighbors[DIR_Z_MINUS] = getNextNodePos(pos, DIR_Z_MINUS);
	return neighbors;
}

int PipeLayer::countAvailableInDirection(Point pos, Direction dir) {
	int maxWalk = 0;
	switch(dir) {
		case DIR_X_PLUS: maxWalk = node_struct_size.x - pos.x; break;
		case DIR_X_MINUS: maxWalk = pos.x; break;
		case DIR_Y_PLUS: maxWalk = node_struct_size.y - pos.y; break;
		case DIR_Y_MINUS: maxWalk = pos.y; break;
		case DIR_Z_PLUS: maxWalk = node_struct_size.z - pos.z; break;
		case DIR_Z_MINUS: maxWalk = pos.z; break;
		case DIR_NONE: break;
	}
	int nodeCount = 0;
	Point testNode = pos;

	for(int i = 0; i < maxWalk; i++) {
		testNode = getNextNodePos(testNode, dir);
		if(!isEmpty(&testNode)) { break; }
		nodeCount++;
	}
	return nodeCount;
	;
}

Direction PipeLayer::chooseRandomDirection(Point pos) {
	Point* neighbors = getNeighbors(pos);
	std::vector<Direction>* emptyDir = new std::vector<Direction>();
	Direction retDir = DIR_NONE;

	//Bit of a brute force method but hey it works
	for(int i = 0; i < 6; i++) {
		if(isEmpty(&neighbors[i])) { emptyDir->push_back((Direction) i); }
	}

	retDir = emptyDir->at(iRand(emptyDir->size()));

	//Be responsible with your memory!
	delete emptyDir;

	return retDir;
}

void PipeLayer::outputPipePath(int pipeIdx) { pipes->outputPipePath(pipeIdx); }

void PipeLayer::drawPipe(int pipeIdx) {
	auto pipe = (*pipes)[pipeIdx];
	auto iter = std::begin(pipe);
	auto end = std::end(pipe);

	while(iter != end) {
		Node* nodePtr = node_struct[iter->x][iter->y][iter->z];
		nodePtr->draw();
		iter++;
	}
}
