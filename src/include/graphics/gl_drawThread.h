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

class DRAW_THREAD {
public:
	int priority;

	DRAW_THREAD();

	/**
	 * @brief Delete any GL contexts
	 * 
	 */
	~DRAW_THREAD();
	PIPE* pPipe;// generic pipe ptr
	void SetPipe(PIPE* pipe);

	/**
	 * @brief Starts up pipe of the approppriate type.  
	 * 
	 * @return true 
	 * @return false If can't find an empty node for the pipe to start on
	 */
	bool StartPipe();

	/**
	 * @brief Draw pipe in thread slot, according to its type
	 */
	void DrawPipe();
	void KillPipe();
};