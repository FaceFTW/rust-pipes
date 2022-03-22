/**
 * @file pipe.h
 * @author Alex "FaceFTW" Westerman
 * @brief Main Definition of a Pipe
 * @version 0.1
 * @date 2022-03-17
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __pipe_h_
#define __pipe_h_

#include "global_state.h"
#include "structs.h"
#include "utils.h"
#include <GL/gl.h>

class State;

class Pipe {
public:
	int type;
	IPOINT3D curPos;// current node position of pipe

	State* pState;// for state value access

	void setChooseStartPosMethod(int method);

	void setChooseDirectionMethod(int method);

	/**
	 * @brief Call direction-finding function based on current method
	 * @return int 
	 */
	int chooseNewDirection();
	bool isStuck();// if pipe is stuck or not
	bool nowhereToRun() { return status == PIPE_OUT_OF_NODES; }
	Pipe();
	Pipe(State* npState);

	/**
	 * @brief Start drawing a new normal pipe
	 * @li Draw a start cap and short pipe in new direction
	 * 
	 */
	void start();

	/**
	 * @brief Decides which elbow to draw
	 * @details Beginning of each elbow is aligned along +y, and we have 
	 * to choose the one with the notch in correct position
	 * - The 'primary' start notch (elbow[0]) is in same direction as
	 * newDir, and successive elbows rotate this notch CCW around +y
	 * 
	 * @param oldDir 
	 * @param newDir 
	 * @return GLint 
	 */
	GLint chooseElbow(int oldDir, int newDir);

	/**
	 * @brief Draw a joint between 2 pipes
	 * 
	 * @param newDir 
	 */
	void drawJoint(int newDir);

	/**
	 * @brief If turning, draws a joint and a short cylinder, otherwise draws a long cylinder.
	 * @details 'current node' is set as the one we draw thru the NEXT time around.
	 */
	void draw();//mf: could take param to draw n sections

	/**
	 * @brief Cap the start of the pipe with a ball
	 * @param newDir 
	 */
	void drawStartCap(int newDir);

	/**
	 * @brief Draws a ball, used to cap end of a pipe
	 */
	void drawEndCap();

protected:
	float radius;// ideal radius (fluctuates for FPIPE)
	int status;  // ACTIVE/STUCK/STOPPED, etc.
	int lastDir; // last direction taken by pipe
	int notchVec;// current notch vector

	int weightStraight;// current weighting of going straight

	/**
	 * @brief Find an empty node to start the pipe on
	 * 
	 * @return true 
	 * @return false 
	 */
	bool setStartPos();// starting node position
	void chooseMaterial();
	void drawTeapot();

	/**
	 * @brief Increment current position according to direction taken
	 * @param dir 
	 */
	void updateCurrentPosition(int dir);
	void translateToCurrentPosition();

private:
	int chooseStartPosMethod;
	int chooseDirMethod;

	// int getBestDirsForChase(int* bestDirs);
};

#endif //__pipe_h_