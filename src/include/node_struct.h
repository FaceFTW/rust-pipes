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

#pragma region "Global Namespace Defines"
/******CONSTANTS*******/
const int NUM_DIR = 6;

/******ENUMS*******/
typedef enum e_axis { AXIS_X, AXIS_Y, AXIS_Z } Axis;
typedef enum e_direction {
	DIR_X_PLUS = 0,
	DIR_X_MINUS = 1,
	DIR_Y_PLUS = 2,
	DIR_Y_MINUS = 3,
	DIR_Z_PLUS = 4,
	DIR_Z_MINUS = 5,
	DIR_NONE
} Direction;

/******STRUCTS*******/
typedef struct s_point_t {
	uint x;
	uint y;
	uint z;

	s_point_t(uint _x, uint _y, uint _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	s_point_t(s_point_t* _point) {
		x = _point->x;
		y = _point->y;
		z = _point->z;
	}

	//Neighbor Constructor
	s_point_t(s_point_t* _point, Direction dir) {
		switch(dir) {
			case DIR_X_PLUS:
				x = (_point->x) + 1;
				y = (_point->y);
				z = (_point->z);
				break;
			case DIR_X_MINUS:
				x = (_point->x) - 1;
				y = (_point->y);
				z = (_point->z);
				break;
			case DIR_Y_PLUS:
				x = (_point->x);
				y = (_point->y) + 1;
				z = (_point->z);
				break;
			case DIR_Y_MINUS:
				x = (_point->x);
				y = (_point->y) - 1;
				z = (_point->z);
				break;
			case DIR_Z_PLUS:
				x = (_point->x);
				y = (_point->y);
				z = (_point->z) + 1;
				break;
			case DIR_Z_MINUS:
				x = (_point->x);
				y = (_point->y);
				z = (_point->z) - 1;
				break;
		}
	}

} Point;

/******TYPEDEFS*******/
template<class T> using NodeVector3 = std::vector<std::vector<std::vector<T*>>>;
template<class T> using NodeVector = std::vector<T>;

#pragma endregion

template<class T> class NodeStruct {
public:
	NodeStruct();// Constructor
	NodeStruct(Point* size);
	~NodeStruct();// Destructor

	T& operator()(uint x, uint y, uint z);// Matrix Operator (Reference)
	T& operator[](Point* pos);

	Point* size();
	uint size(Axis d);

	bool isEmpty(Point* pos);

	Point** getNeighbors(Point* pos);
	// NodeVector<T>* getNeighbors(uint x, uint y, uint z);

	/**
	 * @brief Choose randomnly among the possible directions.
	 * The likelyhood of going straight is controlled by weighting it.
	 *
	 * @param pos The output direction? TBD
	 * @param dir
	 * @param weight
	 * @return int
	 */
	Direction chooseRandomDirection(Point* pos, Direction dir, int weight);

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
	Direction choosePreferredDirection(Point* pos, Direction dir, Direction* prefDirs, int nPrefDirs);

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
	int chooseNewTurnDirection(IPOINT3D* pos, int dir);

	/**
	 * @brief Finds the direction with the most empty nodes in a line 'searchRadius' long.
	 * Does not mark any nodes as taken.
	 *
	 * @param pos
	 * @return int
	 */
	int findClearestDirection(IPOINT3D* pos);

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
	int getBestPossibleTurns(IPOINT3D* pos, int dir, int* turnDirs);

	/**
	 * @brief Search for an empty node to start drawing
	 * - Marks node as taken (mf: renam fn to ChooseEmptyNode ?
	 *
	 * @param ip3dEmpty Return position of empty node
	 * @return true
	 * @return false if couldn't find a node
	 */
	bool findRandomEmptyNode(IPOINT3D* ip3dEmpty);

	/**
	 * @brief Like FindRandomEmptyNode, but limits search to a 2d plane of the supplied box.
	 *
	 * @param pos
	 * @param plane
	 * @param box
	 * @return true
	 * @return false
	 */
	bool findRandomEmptyNode2D(IPOINT3D* pos, int plane, int* box);

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
	bool takeClosestEmptyNode(IPOINT3D* newPos, IPOINT3D* pos);

private:
	T**** node_struct;// For a 3-dimensional array (better as pointers imho)
	Point* node_struct_size;

	/**
	 * @brief Get ptr to next node from pos and dir
	 *
	 * @param pos Position of the Origin Node
	 * @param dir The direction of the next node
	 * @return Node* Pointer to the next node in the specified direction from the original node
	 */
	// T* getNextNode(Point* pos, Direction dir);

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

	//TODO Update Docs
	/**
	 * @brief Get list of direction indices of empty node neighbours, and put them in supplied matrix
	 * Don't include going straight
	 *
	 * @param nNode
	 * @param nEmpty
	 * @param lastDir
	 * @return int number of empty node neighbours
	 */
	int getEmptyTurnNeighbors(Point* pos, Direction* emptyDirs, Direction lastDir);

	//TODO Update Docs
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
	int getEmptyNeighborsAlongDir(Point* pos, Direction dir, int searchRadius);
};
}// namespace GlPipes

#endif