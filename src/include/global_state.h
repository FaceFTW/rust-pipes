/**
 * @file global_state.h
 * @author Alex "FaceFTW" Westerman
 * @brief Defines a "global" state object for keeping track of multiple pipes
 * This is based on a combination of the STATE and NORMAL_STATE class definitions of the original
 * source code.
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */
#pragma once

#ifndef __state_h__
#define __state_h__

#include "constants.h"
#include "graphics/gl_drawThread.h"
#include "node_array.h"
#include "objects/balljoint_object.h"
#include "objects/elbow_object.h"
#include "objects/pipe_object.h"
#include "objects/sphere_object.h"
#include "pipe.h"
#include "structs.h"

// Reset status

#define RESET_STARTUP_BIT (1L << 0)
#define RESET_NORMAL_BIT (1L << 1)
#define RESET_RESIZE_BIT (1L << 2)
#define RESET_REPAINT_BIT (1L << 3)
#define RESET_BIT (0xF)

class Pipe;
class DrawThread;

class State {
public:
	int nSlices;// reference # of slices around a pipe
	int nTextures;

	GLConfig glCfg;
	float radius;     // 'reference' pipe radius value
	NODE_ARRAY* nodes;// for keeping track of draw space

	/**
	 * @brief Construct a new STATE object
	 * - global state init
	 * - translates variables set from the dialog boxes
	 */
	State();

	/**
	 * @brief 
	 * Some of the objects are always created, so don't have to check if they
	 * exist. Others may be NULL.
	 * 
	 */
	~State();
	//void reshape(int width, int height, void* data);
	void repaint(void* data);
	/**
	 * @brief Top-level pipe drawing routine
	 * @details Each pipe thread keeps drawing new pipes until we reach maximum number
	 *   of pipes per frame - then each thread gets killed as soon as it gets
	 *   stuck.  Once number of drawing threads reaches 0, we start a new
	 *   frame
	 * 
	 * @param data 
	 */
	void draw();

	/**
* - 
	 * @brief Called when GL window being closed
	 * 
	 * @param data 
	 */
	void finish(void* data);

	//NSTATE stuff that I ported Over
	int jointStyle;
	int bCycleJointStyles;

	PipeObject* shortPipe;
	PipeObject* longPipe;
	ElbowObject* elbows[4];
	SphereObject* ballCap;
	SphereObject* bigBall;
	BalljointObject* ballJoints[4];

	/**
	 * @brief Reset frame attributes for normal pipes.
	 */
	void reset();

	/**
	 * @brief Build all the pipe primitives
	 * 
	 * @param radius 
	 * @param divSice 
	 * @param nSlices 
	 */
	void buildObjects(float radius, float divSice, int nSlices);

	/**
	 * @brief Decides which type of joint to draw  
	 * @return int 
	 */
	int chooseJointType();

private:
	int maxPipesPerFrame;// max number of separate pipes/frame
	int nPipesDrawn;     // number of pipes drawn or drawing in frame
	int maxDrawThreads;  // max number of concurrently drawing pipes
	int nDrawThreads;    // number of live threads
	DrawThread drawThreads[MAX_DRAW_THREADS];

	int resetStatus;

	//SS_DIGITAL_DISSOLVE_CLEAR ddClear;
	//int         bCalibrateClear;
	void drawValidate();// validation to do before each Draw

	/**
	 * @brief Called on FrameReset resulting from change in viewing paramters (e.g. from a Resize event).
	 */
	void resetView();

	/**
	 * @brief 
	 * 
	 */
	void frameReset();

	/**
	 * @brief Clear the screen.
	 */
	void clear();

	/**
	 * @brief Compact the thread list according to number of pipe threads killed
	 * @details @li The pipes have been killed, but the RC's in each slot are still valid
 	 *   and reusable.  So we swap up entries with valid pipes. This means that
	 *   the ordering of the RC's in the thread list will change during the life
	 *   of the program.  This should be OK.
	 * 
	 */
	void compactThreadList();
};

#endif// __state_h__
