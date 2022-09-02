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

#include "pipe.h"
#include "utils.h"
#include <vector>

namespace GlPipes {

#pragma region "Global Namespace Defines"

#pragma endregion

class PipeLayer {
public:
	PipeLayer();// Constructor
	PipeLayer(Point* node_size, int numPipes);
	~PipeLayer();// Destructor

	/**
	 * @brief Accesses a Pipe object
	 *
	 * @param pipeIdx Index of the pipe in the array
	 * @return Pipe&
	 */
	Pipe*& operator()(int pipeIdx);

	/**
	 * @brief Accesses a PipePart
	 *
	 * @param pos
	 * @return PipePart&
	 */
	PipePart*& operator[](Point* pos);

	Point* size();
	uint size(Axis d);

	bool isEmpty(Point* pos);

	void generatePipe(int pipeIdx);

	Point** getNeighbors(Point* pos);

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

	Direction chooseRandomInitialDirection(Point* pos);

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
	 ** NOTE: This description is slightly outdated, as it passes in pointers
	 ** that will effectively be used to define two pipe joints (one PIPE_JOINT, one PIPE_SEG)
	 *
	 * @param pos
	 * @param dir
	 * @return int
	 */
	Direction chooseNewTurnDirection(Point* pos, Point* jointPos, Point* postTurnPos,
	                                 Direction prevDir);

	/**
	 * @brief Search for an empty node to start drawing
	 * - Marks node as taken (mf: renam fn to ChooseEmptyNode ?
	 *
	 * @param ip3dEmpty Return position of empty node
	 * @return true
	 * @return false if couldn't find a node
	 */
	Point* findRandomEmptyNode();

private:
	PipePart**** node_struct;// For a 3-dimensional array (better as pointers imho)
	Pipe** pipes;
	Point* node_struct_size;

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
};

}// namespace GlPipes

#endif