/**
 * @file pipe_normal.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definition of Normal Pipes
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

//NOTE this is mostly a copy of NPIPE.h for now, will likely change over time

#ifndef __pipe_normal_h__
#define __pipe_normal_h__


#include "../state/state.h"
#include "pipe.h"

class NORMAL_STATE;

class NORMAL_PIPE : public PIPE {
	private:
	public:
	NORMAL_STATE* pNState;

	/**
	 * @brief Construct a new normal pipe object
	 * 
	 * @param state 
	 */
	NORMAL_PIPE(STATE* state);

	/**
	 * @brief Start drawing a new normal pipe
	 * Draw a start cap and short pipe in new direction
	 */
	void        Start();

	/**
	 * @brief Decides which elbow to draw
	 * The beginning of each elbow is aligned along +y, and we have to choose the one with the notch in correct position.
	 * The 'primary' start notch (elbow[0]) is in same direction as newDir, and successive elbows rotate this notch CCW around +y
	 *
	 * @param oldDir
	 * @param newDir
	 * @return GLint
	 */
	GLint       ChooseElbow(int oldDir, int newDir);

	/**
	 * @brief Draw a joint between 2 pipes
	 *
	 * @param newDir
	 */
	void        DrawJoint(int newDir);

	/**
	 * If turning, draws a joint and a short cylinder, otherwise draws a long cylinder.
	 * The 'current node' is set as the one we draw thru the NEXT time around.
	 *
	 */
	void        Draw(); //mf: could take param to draw n sections

	/**
	 * @brief Cap the start of the pipe with a ball
	 *
	 * @param newDir
	 */
	void        DrawStartCap(int newDir);

	/**
	 * @brief Draws a ball, used to cap end of a pipe
	 *
	 */
	void        DrawEndCap();
};

#endif // __pipe_normal_h__
