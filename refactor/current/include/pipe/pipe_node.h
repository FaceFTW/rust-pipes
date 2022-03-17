/**
 * @file pipe_node.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

//NOTE This is mostly just a carbon copy of NODE.H for now, might not need changes

#ifndef __pipe_node_h__
#define __pipe_node_h__

//TODO remove any needed bindings (seems like none relevant)
//#include "sscommon.h"
#include "../pipe_main.h"

#define NUM_NODE (NUM_DIV - 1)  // num nodes in longest dimension

// maximum weighting of going straight for direction choosing functions
#define MAX_WEIGHT_STRAIGHT 100

// Node class

class Node {
	public:
	void        MarkAsTaken() { empty = FALSE; }
	void        MarkAsEmpty() { empty = TRUE; }
	BOOL        IsEmpty() { return empty; }
	private:
	GLboolean   empty;
};

/**************************************************************************\
*
* Node array class
*
* - 3d array of nodes
* - Functions to access node neighbours, query if taken or not, etc.
* - Not only is this the node array, but a set of methods that operates on it
*
\**************************************************************************/

class NODE_ARRAY {
	public:
	NODE_ARRAY();
	~NODE_ARRAY();
	void        Resize(IPOINT3D* pNewSize); // new array size
	void        Reset();       // set all nodes to empty
	int         ChooseRandomDirection(IPOINT3D* pos, int dir, int weight);
	int         ChoosePreferredDirection(IPOINT3D* pos, int dir, int* prefDirs,
										  int nPrefDirs);
	int         ChooseNewTurnDirection(IPOINT3D* pos, int dir);
	int         FindClearestDirection(IPOINT3D* pos);
	int         GetBestPossibleTurns(IPOINT3D* pos, int dir, int* turnDirs);
	BOOL        FindRandomEmptyNode(IPOINT3D* ip3dEmpty);
	BOOL        FindRandomEmptyNode2D(IPOINT3D* pos, int plane, int* box);
	BOOL        TakeClosestEmptyNode(IPOINT3D* newPos, IPOINT3D* pos);
	void        NodeVisited(IPOINT3D* pos);
	void        GetNodeCount(IPOINT3D* pos);
	private:
	Node* nodes;         // ptr to node array
	int         lock;          // semaphore lock for >1 drawing pipes
	IPOINT3D    numNodes;      // x,y,z dimensions of node array
	int         nodeDirInc[NUM_DIRS]; // array offset between nodes for each dir
	void        GetNeighbours(IPOINT3D* pos, Node** nNode);
	Node* GetNode(IPOINT3D* pos);
	Node* GetNextNode(IPOINT3D* pos, int dir);
	BOOL        GetNextNodePos(IPOINT3D* curPos, IPOINT3D* nextPos, int dir);
	int         GetEmptyNeighbours(Node** nNode, int* nEmpty);
	int         GetEmptyTurnNeighbours(Node** nNode, int* nEmpty, int lastDir);
	int         GetEmptyNeighboursAlongDir(IPOINT3D* pos, int dir,
									int searchRadius);
};

#endif // __pipe_node_h__
