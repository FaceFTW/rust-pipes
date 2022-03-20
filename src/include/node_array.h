/**
 * @file node_array.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definition for the Node Arrays
 * Copied from the originals' NODE and NODE_ARRAY Definitons
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __node_array_h__
#define __node_array_h__

// #include "sscommon.h"
// #include "sspipes.h"
#include "constants.h"
#include "structs.h"
#include <GL/gl.h>

// Node class

class Node {
public:
	void MarkAsTaken() { empty = false; }
	void MarkAsEmpty() { empty = true; }
	bool IsEmpty() { return empty; }

private:
	GLboolean empty;//Defines if a node is occupied
};

/**
 * @brief Node array class
 * @details description
 * - 3d array of nodes
 * - Functions to access node neighbours, query if taken or not, etc. 
 * - Not only is this the node array, but a set of methods that operates on it
 */
class NODE_ARRAY {
public:
	/**
	 * @brief Construct a new node array object
	 */
	NODE_ARRAY();

	/**
	 * @brief Destroy the node array object
	 */
	~NODE_ARRAY();

	/**
	 * @brief Resize the internal array by the specified dimensions
	 * @param pNewSize The new dimensions of the array
	 */
	void Resize(IPOINT3D* pNewSize);//

	/**
	 * @brief Set all nodes to empty
	 */
	void Reset();

	/**
	 * @brief Choose randomnly among the possible directions.  
	 * The likelyhood of going straight is controlled by weighting it.
	 * 
	 * @param pos The output direction? TBD
	 * @param dir 
	 * @param weight 
	 * @return int 
	 */
	int ChooseRandomDirection(IPOINT3D* pos, int dir, int weight);

	/**
	 * @brief  Choose randomnly from one of the supplied preferred directions.
	 * If none of these are available, then try and choose any empty direction
	 * 
	 * @param pos 
	 * @param dir 
	 * @param prefDirs 
	 * @param nPrefDirs 
	 * @return int 
	 */
	int ChoosePreferredDirection(IPOINT3D* pos, int dir, int* prefDirs,
	                             int nPrefDirs);

	/**
	 * @brief Choose a direction to turn
	 * This requires finding a pair of nodes to turn through.  The first node
	 * is in the direction of the turn from the current node, and the second node
	 * is at right angles to this at the end position.  The prim will not draw
	 * through the first node, but may sweep close to it, so we have to mark it
	 * as taken.
	 * 
	 * - if next node is free, but there are no turns available, return
	 *   DIR_STRAIGHT, so the caller can decide what to do in this case
	 * - The turn possibilities are based on the orientation of the current xc, with
	 *   4 relative directions to seek turns in.
	 * 
	 * @param pos 
	 * @param dir 
	 * @return int 
	 */
	int ChooseNewTurnDirection(IPOINT3D* pos, int dir);

	/**
	 * @brief Finds the direction with the most empty nodes in a line 'searchRadius' long.
	 * Does not mark any nodes as taken.
	 * 
	 * @param pos 
	 * @return int 
	 */
	int FindClearestDirection(IPOINT3D* pos);

	/**
	 * @brief From supplied direction and position, figure out which of 4 possible directions are best to turn in.
	 * 
	 * Turns that have the greatest number of empty nodes after the turn are the
	 * best, since a pipe is less likely to hit a dead end in this case.
	 * - We only check as far as 'searchRadius' nodes along each dir.
	 * - Return direction indices of best possible turns in turnDirs, and return 
	 *   count of these turns in fuction return value.
	 * 
	 * @param pos 
	 * @param dir 
	 * @param turnDirs 
	 * @return int 
	 */
	int GetBestPossibleTurns(IPOINT3D* pos, int dir, int* turnDirs);

	/**
	 * @brief Search for an empty node to start drawing
	 * - Marks node as taken (mf: renam fn to ChooseEmptyNode ?
	 * 
	 * @param ip3dEmpty Return position of empty node
	 * @return true 
	 * @return false if couldn't find a node
	 */
	bool FindRandomEmptyNode(IPOINT3D* ip3dEmpty);

	/**
	 * @brief Like FindRandomEmptyNode, but limits search to a 2d plane of the supplied box.
	 * 
	 * @param pos 
	 * @param plane 
	 * @param box 
	 * @return true 
	 * @return false 
	 */
	bool FindRandomEmptyNode2D(IPOINT3D* pos, int plane, int* box);

	/**
	 * @brief Search for an empty node closest to supplied node position
	 * - Marks node as taken
	 * - not completely optimized - if when dilating the box, a side gets
	 *   clamped against the node array, this side will continue to be searched
	 * 
	 * @param newPos 
	 * @param pos 
	 * @return true 
	 * @return false if couldn't find a node
	 */
	bool TakeClosestEmptyNode(IPOINT3D* newPos, IPOINT3D* pos);

	/**
	 * @brief Mark the node as non-empty
	 * 
	 * @param pos The node at position pos to mark as non-empty
	 */
	void NodeVisited(IPOINT3D* pos);

	/**
	 * @brief Get the Node Count object
	 * 
	 * @param pos 
	 */
	void GetNodeCount(IPOINT3D* pos);

private:
	Node* nodes;             // ptr to node array
	int lock;                // semaphore lock for >1 drawing pipes
	IPOINT3D numNodes;       // x,y,z dimensions of node array
	int nodeDirInc[NUM_DIRS];// array offset between nodes for each dir

	/**
	 * @brief Get neigbour nodes relative to supplied position
	 *
	 * - get addresses of the neigbour nodes,
	 *   and put them in supplied matrix
	 * - boundary hits are returned as NULL
	 * 
	 * @param pos 
	 * @param nNode 
	 */
	void GetNeighbours(IPOINT3D* pos, Node** nNode);

	/**
	 * @brief Get ptr to node from position
	 * 
	 * @param pos The position of the node
	 * @return Node* Pointer to the Node object at that position
	 */
	Node* GetNode(IPOINT3D* pos);

	/**
	 * @brief Get ptr to next node from pos and dir
	 * 
	 * @param pos Position of the Origin Node
	 * @param dir The direction of the next node
	 * @return Node* Pointer to the next node in the specified direction from the original node
	 */
	Node* GetNextNode(IPOINT3D* pos, int dir);

	/**
	 * @brief Get position of next node from curPos and lastDir
	 * 
	 * @param curPos Pointer to origin node position
	 * @param nextPos Pointer to next node position
	 * @param dir Direction to find the next node
	 * @return true if Next Node exists
	 * @return false if boundary hit or node empty
	 */
	bool GetNextNodePos(IPOINT3D* curPos, IPOINT3D* nextPos, int dir);

	/**
	 * @brief Get list of direction indices of empty node neighbours, and put them in supplied matrix
	 * 
	 * @param nNode 
	 * @param nEmpty 
	 * @return int number of empty node neighbours
	 */
	int GetEmptyNeighbours(Node** nNode, int* nEmpty);

	/**
	 * @brief Get list of direction indices of empty node neighbours, and put them in supplied matrix
	 * Don't include going straight
	 * 
	 * @param nNode 
	 * @param nEmpty 
	 * @param lastDir 
	 * @return int number of empty node neighbours
	 */
	int GetEmptyTurnNeighbours(Node** nNode, int* nEmpty, int lastDir);

	/**
	 * @brief Sort of like GetEmptyNeighbours, but just gets one neigbour according to supplied dir
	 *
	 * Given a position and direction, find out how many contiguous empty nodes 
	 * there are in that direction.
	 * @param pos 
	 * @param dir 
	 * @param searchRadius Limits Search to specified radius
	 * @return int Contiguous empty node count
	 */
	int GetEmptyNeighboursAlongDir(IPOINT3D* pos, int dir,
	                               int searchRadius);
};

/**
 * @brief Increase box radius without exceeding bounds
 * 
 * @param box 
 * @param bounds 
 */
static void DilateBox(int* box, IPOINT3D* bounds);

#endif// __node_array_h__
