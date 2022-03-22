/**
 * @file node_array.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief Implementation of a Node Array used by the Global State
 * This is copied from the NODE.CXX file from the original source with modifications.
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "include/node_array.h"
#include "include/utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
// #include <windows.h>

// #include "node.h"
// #include "sspipes.h"

NODE_ARRAY::NODE_ARRAY() {
	nodes = NULL;// allocated on Resize

	numNodes.x = 0;
	numNodes.y = 0;
	numNodes.z = 0;
}

NODE_ARRAY::~NODE_ARRAY() {
	if(nodes)
		delete nodes;
}

void NODE_ARRAY::resize(IPOINT3D* pNewSize) {
	if((numNodes.x == pNewSize->x) &&
	   (numNodes.y == pNewSize->y) &&
	   (numNodes.z == pNewSize->z))
		return;

	numNodes = *pNewSize;

	int elemCount = numNodes.x * numNodes.y * numNodes.z;

	if(nodes)
		delete nodes;

	nodes = new Node[elemCount];

	//SS_ASSERT(nodes, "NODE_ARRAY::Resize : can't alloc nodes\n");

	// Reset the node states to empty

	int i;
	Node* pNode = nodes;
	for(i = 0; i < elemCount; i++, pNode++)
		pNode->MarkAsEmpty();

	// precalculate direction offsets between nodes for speed
	nodeDirInc[PLUS_X] = 1;
	nodeDirInc[MINUS_X] = -1;
	nodeDirInc[PLUS_Y] = numNodes.x;
	nodeDirInc[MINUS_Y] = -nodeDirInc[PLUS_Y];
	nodeDirInc[PLUS_Z] = numNodes.x * numNodes.y;
	nodeDirInc[MINUS_Z] = -nodeDirInc[PLUS_Z];
}

void NODE_ARRAY::reset() {
	int i;
	Node* pNode = nodes;

	// Reset the node states to empty
	for(i = 0; i < (numNodes.x) * (numNodes.y) * (numNodes.z); i++, pNode++)
		pNode->MarkAsEmpty();
}

void NODE_ARRAY::getNodeCount(IPOINT3D* count) {
	*count = numNodes;
}

int NODE_ARRAY::chooseRandomDirection(IPOINT3D* pos, int dir, int weightStraight) {
	Node* nNode[NUM_DIRS];
	int numEmpty, newDir;
	int choice;
	Node* straightNode = NULL;
	int emptyDirs[NUM_DIRS];

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::ChooseRandomDirection: invalid dir\n");

	// Get the neigbouring nodes
	getNeighbours(pos, nNode);

	// Get node in straight direction if necessary
	if(weightStraight && nNode[dir] && nNode[dir]->IsEmpty()) {
		straightNode = nNode[dir];
		// if maximum weight, choose and return
		if(weightStraight == MAX_WEIGHT_STRAIGHT) {
			straightNode->MarkAsTaken();
			return dir;
		}
	} else
		weightStraight = 0;

	// Get directions of possible turns
	numEmpty = getEmptyTurnNeighbours(nNode, emptyDirs, dir);

	// Make a random choice
	if((choice = (weightStraight + numEmpty)) == 0)
		return DIR_NONE;
	choice = iRand(choice);

	if(choice < weightStraight) {
		straightNode->MarkAsTaken();
		return dir;
	} else {
		// choose one of the turns
		newDir = emptyDirs[choice - weightStraight];
		nNode[newDir]->MarkAsTaken();
		return newDir;
	}
}

int NODE_ARRAY::choosePreferredDirection(IPOINT3D* pos, int dir, int* prefDirs,
                                         int nPrefDirs) {
	Node* nNode[NUM_DIRS];
	int numEmpty, newDir;
	int emptyDirs[NUM_DIRS];
	int* pEmptyPrefDirs;
	int i, j;

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::ChoosePreferredDirection : invalid dir\n");

	// Get the neigbouring nodes
	getNeighbours(pos, nNode);

	// Create list of directions that are both preferred and empty

	pEmptyPrefDirs = emptyDirs;
	numEmpty = 0;

	for(i = 0, j = 0; (i < NUM_DIRS) && (j < nPrefDirs); i++) {
		if(i == *prefDirs) {
			prefDirs++;
			j++;
			if(nNode[i] && nNode[i]->IsEmpty()) {
				// add it to list
				*pEmptyPrefDirs++ = i;
				numEmpty++;
			}
		}
	}

	// if no empty preferred dirs, then any empty dirs become preferred

	if(!numEmpty) {
		numEmpty = getEmptyNeighbours(nNode, emptyDirs);
		if(numEmpty == 0)
			return DIR_NONE;
	}

	// Pick a random dir from the empty set

	newDir = emptyDirs[iRand(numEmpty)];
	nNode[newDir]->MarkAsTaken();
	return newDir;
}

int NODE_ARRAY::findClearestDirection(IPOINT3D* pos) {
	static Node* neighbNode[NUM_DIRS];
	static int emptyDirs[NUM_DIRS];
	int nEmpty, newDir;
	int maxEmpty = 0;
	int searchRadius = 3;
	int count = 0;
	int i;

	// Get ptrs to neighbour nodes

	getNeighbours(pos, neighbNode);

	// find empty nodes in each direction

	for(i = 0; i < NUM_DIRS; i++) {
		if(neighbNode[i] && neighbNode[i]->IsEmpty()) {
			// find number of contiguous empty nodes along this direction
			nEmpty = getEmptyNeighboursAlongDir(pos, i, searchRadius);
			if(nEmpty > maxEmpty) {
				// we have a new winner
				count = 0;
				maxEmpty = nEmpty;
				emptyDirs[count++] = i;
			} else if(nEmpty == maxEmpty) {
				// tied with current max
				emptyDirs[count++] = i;
			}
		}
	}

	if(count == 0)
		return DIR_NONE;

	// randomnly choose a direction
	newDir = emptyDirs[iRand(count)];

	return newDir;
}

int NODE_ARRAY::chooseNewTurnDirection(IPOINT3D* pos, int dir) {
	int turns[NUM_DIRS], nTurns;
	IPOINT3D nextPos;
	int newDir;
	Node* nextNode;

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::ChooseNewTurnDirection : invalid dir\n");

	// First, check if next node along current dir is empty

	if(!getNextNodePos(pos, &nextPos, dir))
		return DIR_NONE;// node out of bounds or not empty

	// Ok, the next node is free - check the 4 possible turns from here

	nTurns = getBestPossibleTurns(&nextPos, dir, turns);
	if(nTurns == 0)
		return DIR_STRAIGHT;// nowhere to turn, but could go straight

	// randomnly choose one of the possible turns
	newDir = turns[iRand(nTurns)];

	// SS_ASSERT((newDir >= 0) && (newDir < NUM_DIRS),
	//           "NODE_ARRAY::ChooseNewTurnDirection : invalid newDir\n");

	// mark taken nodes

	nextNode = getNode(&nextPos);
	nextNode->MarkAsTaken();

	nextNode = getNextNode(&nextPos, newDir);
	nextNode->MarkAsTaken();

	return newDir;
}

int NODE_ARRAY::getBestPossibleTurns(IPOINT3D* pos, int dir, int* turnDirs) {
	Node* neighbNode[NUM_DIRS];// ptrs to 6 neighbour nodes
	int i, count = 0;
	bool check[NUM_DIRS] = {true, true, true, true, true, true};
	int nEmpty, maxEmpty = 0;
	int searchRadius = 2;

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::GetBestPossibleTurns : invalid dir\n");

	getNeighbours(pos, neighbNode);

	switch(dir) {
		case PLUS_X:
		case MINUS_X:
			check[PLUS_X] = false;
			check[MINUS_X] = false;
			break;
		case PLUS_Y:
		case MINUS_Y:
			check[PLUS_Y] = false;
			check[MINUS_Y] = false;
			break;
		case PLUS_Z:
		case MINUS_Z:
			check[PLUS_Z] = false;
			check[MINUS_Z] = false;
			break;
	}

	// check approppriate directions
	for(i = 0; i < NUM_DIRS; i++) {
		if(check[i] && neighbNode[i] && neighbNode[i]->IsEmpty()) {
			// find number of contiguous empty nodes along this direction
			nEmpty = getEmptyNeighboursAlongDir(pos, i, searchRadius);
			if(nEmpty > maxEmpty) {
				// we have a new winner
				count = 0;
				maxEmpty = nEmpty;
				turnDirs[count++] = i;
			} else if(nEmpty == maxEmpty) {
				// tied with current max
				turnDirs[count++] = i;
			}
		}
	}

	return count;
}

void NODE_ARRAY::getNeighbours(IPOINT3D* pos, Node** nNode) {
	Node* centerNode = getNode(pos);

	nNode[PLUS_X] = pos->x == (numNodes.x - 1) ? NULL : centerNode + nodeDirInc[PLUS_X];
	nNode[PLUS_Y] = pos->y == (numNodes.y - 1) ? NULL : centerNode + nodeDirInc[PLUS_Y];
	nNode[PLUS_Z] = pos->z == (numNodes.z - 1) ? NULL : centerNode + nodeDirInc[PLUS_Z];

	nNode[MINUS_X] = pos->x == 0 ? NULL : centerNode + nodeDirInc[MINUS_X];
	nNode[MINUS_Y] = pos->y == 0 ? NULL : centerNode + nodeDirInc[MINUS_Y];
	nNode[MINUS_Z] = pos->z == 0 ? NULL : centerNode + nodeDirInc[MINUS_Z];
}

void NODE_ARRAY::nodeVisited(IPOINT3D* pos) {
	(getNode(pos))->MarkAsTaken();
}

Node* NODE_ARRAY::getNode(IPOINT3D* pos) {
	return nodes +
	        pos->x +
	        pos->y * numNodes.x +
	        pos->z * numNodes.x * numNodes.y;
}

Node* NODE_ARRAY::getNextNode(IPOINT3D* pos, int dir) {
	Node* curNode = getNode(pos);

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::GetNextNode : invalid dir\n");

	switch(dir) {
		case PLUS_X:
			return (pos->x == (numNodes.x - 1) ? NULL : curNode + nodeDirInc[PLUS_X]);
			break;
		case MINUS_X:
			return (pos->x == 0 ? NULL : curNode + nodeDirInc[MINUS_X]);
			break;
		case PLUS_Y:
			return (pos->y == (numNodes.y - 1) ? NULL : curNode + nodeDirInc[PLUS_Y]);
			break;
		case MINUS_Y:
			return (pos->y == 0 ? NULL : curNode + nodeDirInc[MINUS_Y]);
			break;
		case PLUS_Z:
			return (pos->z == (numNodes.z - 1) ? NULL : curNode + nodeDirInc[PLUS_Z]);
			break;
		case MINUS_Z:
			return (pos->z == 0 ? NULL : curNode + nodeDirInc[MINUS_Z]);
			break;
		default:
			return NULL;
	}
}

bool NODE_ARRAY::getNextNodePos(IPOINT3D* curPos, IPOINT3D* nextPos, int dir) {
	static Node* neighbNode[NUM_DIRS];// ptrs to 6 neighbour nodes

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::GetNextNodePos : invalid dir\n");

	// don't need to get all neighbours, just one in next direction
	getNeighbours(curPos, neighbNode);

	*nextPos = *curPos;

	// bail if boundary hit or node not empty
	if((neighbNode[dir] == NULL) || !neighbNode[dir]->IsEmpty())
		return false;

	switch(dir) {
		case PLUS_X:
			nextPos->x = curPos->x + 1;
			break;

		case MINUS_X:
			nextPos->x = curPos->x - 1;
			break;

		case PLUS_Y:
			nextPos->y = curPos->y + 1;
			break;

		case MINUS_Y:
			nextPos->y = curPos->y - 1;
			break;

		case PLUS_Z:
			nextPos->z = curPos->z + 1;
			break;

		case MINUS_Z:
			nextPos->z = curPos->z - 1;
			break;
	}

	return true;
}

int NODE_ARRAY::getEmptyNeighbours(Node** nNode, int* nEmpty) {
	int i, count = 0;

	for(i = 0; i < NUM_DIRS; i++) {
		if(nNode[i] && nNode[i]->IsEmpty())
			nEmpty[count++] = i;
	}
	return count;
}

int NODE_ARRAY::getEmptyTurnNeighbours(Node** nNode, int* nEmpty, int lastDir) {
	int i, count = 0;

	for(i = 0; i < NUM_DIRS; i++) {
		if(nNode[i] && nNode[i]->IsEmpty()) {
			if(i == lastDir)
				continue;
			nEmpty[count++] = i;
		}
	}
	return count;
}

int NODE_ARRAY::getEmptyNeighboursAlongDir(IPOINT3D* pos, int dir,
                                           int searchRadius) {
	Node* curNode = getNode(pos);
	int nodeStride;
	int maxSearch;
	int count = 0;

	// SS_ASSERT((dir >= 0) && (dir < NUM_DIRS),
	//           "NODE_ARRAY::GetEmptyNeighboursAlongDir : invalid dir\n");

	nodeStride = nodeDirInc[dir];

	switch(dir) {
		case PLUS_X:
			maxSearch = numNodes.x - pos->x - 1;
			break;
		case MINUS_X:
			maxSearch = pos->x;
			break;
		case PLUS_Y:
			maxSearch = numNodes.y - pos->y - 1;
			break;
		case MINUS_Y:
			maxSearch = pos->y;
			break;
		case PLUS_Z:
			maxSearch = numNodes.z - pos->z - 1;
			break;
		case MINUS_Z:
			maxSearch = pos->z;
			break;
	}

	if(searchRadius > maxSearch)
		searchRadius = maxSearch;

	if(!searchRadius)
		return 0;

	while(searchRadius--) {
		curNode += nodeStride;
		if(!curNode->IsEmpty())
			return count;
		count++;
	}
	return count;
}

bool NODE_ARRAY::findRandomEmptyNode(IPOINT3D* pos) {
	int infLoopDetect = 0;

	while(true) {

		// Pick a random node.

		pos->x = iRand(numNodes.x);
		pos->y = iRand(numNodes.y);
		pos->z = iRand(numNodes.z);

		// If its empty, we're done.

		if(getNode(pos)->IsEmpty()) {
			nodeVisited(pos);
			return true;
		} else {
			// Watch out for infinite loops!  After trying for
			// awhile, give up on the random search and look
			// for the first empty node.

			if(infLoopDetect++ > INFINITE_LOOP) {

				// Search for first empty node.

				for(pos->x = 0; pos->x < numNodes.x; pos->x++)
					for(pos->y = 0; pos->y < numNodes.y; pos->y++)
						for(pos->z = 0; pos->z < numNodes.z; pos->z++)
							if(getNode(pos)->IsEmpty()) {
								nodeVisited(pos);
								return true;
							}

				// There are no more empty nodes.
				// Reset the pipes and exit.

				return false;
			}
		}
	}
}

bool NODE_ARRAY::findRandomEmptyNode2D(IPOINT3D* pos, int plane, int* box) {
	int *newx, *newy;
	int *xDim, *yDim;

	switch(plane) {
		case PLUS_X:
		case MINUS_X:
			pos->x = box[plane];
			newx = &pos->z;
			newy = &pos->y;
			xDim = &box[PLUS_Z];
			yDim = &box[PLUS_Y];
			break;
		case PLUS_Y:
		case MINUS_Y:
			pos->y = box[plane];
			newx = &pos->x;
			newy = &pos->z;
			xDim = &box[PLUS_X];
			yDim = &box[PLUS_Z];
			break;
		case PLUS_Z:
		case MINUS_Z:
			newx = &pos->x;
			newy = &pos->y;
			pos->z = box[plane];
			xDim = &box[PLUS_X];
			yDim = &box[PLUS_Y];
			break;
	}

	int infLoop = 2 * (xDim[MAX_VAL] - xDim[MIN_VAL] + 1) *
	        (yDim[MAX_VAL] - yDim[MIN_VAL] + 1);
	int infLoopDetect = 0;

	while(true) {

		// Pick a random node.

		*newx = iRand2(xDim[MIN_VAL], xDim[MAX_VAL]);
		*newy = iRand2(yDim[MIN_VAL], yDim[MAX_VAL]);

		// If its empty, we're done.

		if(getNode(pos)->IsEmpty()) {
			nodeVisited(pos);
			return true;
		} else {
			// Watch out for infinite loops!  After trying for
			// awhile, give up on the random search and look
			// for the first empty node.

			if(++infLoopDetect > infLoop) {

				// Do linear search for first empty node.

				for(*newx = xDim[MIN_VAL]; *newx <= xDim[MAX_VAL]; (*newx)++)
					for(*newy = yDim[MIN_VAL]; *newy <= yDim[MAX_VAL]; (*newy)++)
						if(getNode(pos)->IsEmpty()) {
							nodeVisited(pos);
							return true;
						}

				// There are no empty nodes in this plane.
				return false;
			}
		}
	}
}

bool NODE_ARRAY::takeClosestEmptyNode(IPOINT3D* newPos, IPOINT3D* pos) {
	static int searchRadius = MAX(numNodes.x, numNodes.y) / 3;

	// easy out
	if(getNode(pos)->IsEmpty()) {
		nodeVisited(pos);
		*newPos = *pos;
		return true;
	}

	int box[NUM_DIRS] = {pos->x, pos->x, pos->y, pos->y, pos->z, pos->z};
	int clip[NUM_DIRS] = {0};

	// do a random search on successively larger search boxes
	for(int i = 0; i < searchRadius; i++) {
		// Increase box size
		dilateBox(box, &numNodes);
		// start looking in random 2D face of the box
		int dir = iRand(NUM_DIRS);
		for(int j = 0; j < NUM_DIRS; j++, dir = (++dir == NUM_DIRS) ? 0 : dir) {
			if(findRandomEmptyNode2D(newPos, dir, box))
				return true;
		}
	}

	// nothing nearby - grab a random one
	return findRandomEmptyNode(newPos);
}

static void dilateBox(int* box, IPOINT3D* bounds) {
	int* min = (int*) &box[MINUS_X];
	int* max = (int*) &box[PLUS_X];
	int* boundMax = (int*) bounds;
	// boundMin always 0

	for(int i = 0; i < 3; i++, min += 2, max += 2, boundMax++) {
		if(*min > 0)
			(*min)--;
		if(*max < (*boundMax - 1))
			(*max)++;
	}
}
