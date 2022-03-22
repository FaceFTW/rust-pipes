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

#ifndef __state_h__
#define __state_h__

#include "constants.h"
#include "graphics/gl_drawThread.h"
#include "node_array.h"
#include "pipe.h"

// Reset status

#define RESET_STARTUP_BIT (1L << 0)
#define RESET_NORMAL_BIT (1L << 1)
#define RESET_RESIZE_BIT (1L << 2)
#define RESET_REPAINT_BIT (1L << 3)

// this used for traditional pipe drawing

class PIPE_OBJECT;
class ELBOW_OBJECT;
class SPHERE_OBJECT;
class BALLJOINT_OBJECT;

// // Program existence instance

class STATE {
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
	STATE();

	/**
	 * @brief 
	 * Some of the objects are always created, so don't have to check if they
	 * exist. Others may be NULL.
	 * 
	 */
	~STATE();
	// void Reshape(int width, int height, void* data);
	// void Repaint(LPRECT pRect, void* data);
	void Draw(void* data);
	void Finish(void* data);

	//NSTATE stuff that I ported Over
	int jointStyle;
	int bCycleJointStyles;

	PIPE_OBJECT* shortPipe;
	PIPE_OBJECT* longPipe;
	ELBOW_OBJECT* elbows[4];
	SPHERE_OBJECT* ballCap;
	SPHERE_OBJECT* bigBall;
	BALLJOINT_OBJECT* ballJoints[4];
	void Reset();
	//    int             GetMaxPipesPerFrame();
	// void BuildObjects(float radius, float divSize, int nSlices,
	//                   BOOL bTexture, IPOINT2D* pTexRep);
	void BuildObjects(float radius, float divSice, int nSlices);
	int ChooseJointType();

private:
	int maxPipesPerFrame;// max number of separate pipes/frame
	int nPipesDrawn;     // number of pipes drawn or drawing in frame
	int maxDrawThreads;  // max number of concurrently drawing pipes
	int nDrawThreads;    // number of live threads
	DRAW_THREAD drawThreads[MAX_DRAW_THREADS];

	int resetStatus;

	//SS_DIGITAL_DISSOLVE_CLEAR ddClear;
	//int         bCalibrateClear;

	//void        GLInit();
	void DrawValidate();// validation to do before each Draw
	void ResetView();
	void FrameReset();

	/**
	 * @brief Clear the screen.
	 * 
	 */
	void Clear();
	void CompactThreadList();
};

#endif// __state_h__
