/**
 * @file gl_drawThread.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definition of the DrawThread Class
 * Separated from the original Source's STATE.CXX file for clarity
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

// Frame draw schemes
#include "../pipe.h"

class DrawThread {
public:
	int priority;

	DrawThread();

	/**
	 * @brief Delete any GL contexts
	 */
	~DrawThread();

	Pipe* pPipe;// generic pipe ptr
	void setPipe(Pipe* pipe);

	/**
	 * @brief Starts up pipe of the approppriate type.  
	 * 
	 * @return true 
	 * @return false If can't find an empty node for the pipe to start on
	 */
	bool startPipe();

	/**
	 * @brief Draw pipe in thread slot, according to its type
	 */
	void drawPipe();
	void killPipe();
};