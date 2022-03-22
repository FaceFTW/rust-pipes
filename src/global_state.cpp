/**
 * @file global_state.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
//#include <windows.h>

#include "include/constants.h"
#include "include/global_state.h"
#include "include/graphics/gl_init.h"
#include "include/graphics/gl_materials.h"
#include "include/structs.h"
#include "include/utils.h"



State::State() {
	// various state values
	resetStatus = RESET_STARTUP_BIT;

	//TODO where this var come from
	// if(ulSurfStyle == SURFSTYLE_WIREFRAME) {
	// 	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// }
	// Initialize GL state for the initial RC (sets texture state, so
	// (must come after LoadTextureFiles())
	initGLMisc();

	// set 'reference' radius value
	radius = 1.0f;

	// convert tesselation from fTesselFact(0.0-2.0) to tessLevel(0-MAX_TESS)
	//TODO where does this come from?
	//For now just set fTesselFact to 1
	float fTesselFact = 1;
	int tessLevel = (int) (fTesselFact * (MAX_TESS + 1) / 2.0001f);
	nSlices = (tessLevel + 2) * 4;

	// Allocate basic NODE_ARRAY
	// NODE_ARRAY size is determined in Reshape (based on window size)
	nodes = new NODE_ARRAY;

	// Set drawing mode, and initialize accordingly.  For now, either all normal
	// or all flex pipes are drawn, but they could be combined later.
	// Can assume here that if there's any possibility that normal pipes
	// will be drawn, State will be initialized so that dlists are
	// built

	// Again, since have either NORMAL or FLEX, set maxPipesPerFrame,
	// maxDrawThreads
	maxDrawThreads = MAX_DRAW_THREADS;
	nDrawThreads = 0;// no active threads yet
	nPipesDrawn = 0;
	// maxPipesPerFrame is set in Reset()

	// initialize materials
	InitTeaMaterials();

	// init joint types from dialog settings

	bCycleJointStyles = 0;

	//TODO where????
	//For now just init to 
	int ulJointType = JOINT_CYCLE;
	switch(ulJointType) {
		case JOINT_ELBOW:
			jointStyle = ELBOWS;
			break;
		case JOINT_BALL:
			jointStyle = BALLS;
			break;
		case JOINT_MIXED:
			jointStyle = EITHER;
			break;
		case JOINT_CYCLE:
			bCycleJointStyles = 1;
			jointStyle = EITHER;
			break;
		default:
			break;
	}

	// Build the objects
	buildObjects(radius, glCfg.divSize, nSlices);
}

State::~State() {
	if(nodes)
		delete nodes;

	DrawThread* pdt = &drawThreads[0];

	//From NORMAL_State
	delete shortPipe;
	delete longPipe;
	delete ballCap;

	for(int i = 0; i < 4; i++) {
		delete elbows[i];
		if(ballJoints[i])
			delete ballJoints[i];
	}

	if(bigBall)
		delete bigBall;
}

/**************************************************************************\
* Repaint
*
* This is called when a WM_PAINT msg has been sent to the window.   The paint
* will overwrite the frame buffer, screwing up the scene if pipes is in single
* buffer mode.  We set resetStatus accordingly to clear things up on next
* draw. 
*
\**************************************************************************/

// void State::Repaint(LPRECT  void* data) {
// 	resetStatus |= RESET_REPAINT_BIT;
// }

void State::resetView() {
	IPOINT3D numNodes;

	// Have VIEW calculate the node array size based on view params
	calcNodeArraySize(&glCfg, &numNodes);

	// Resize the node array
	nodes->resize(&numNodes);
}


void State::frameReset() {
	int i;
	float xRot, zRot;
	Pipe* pNewPipe;

#ifdef DO_TIMING
	Timer(TIMER_STOP);
#endif

	// DBGINFO("Pipes State::FrameReset:\n");

	// Kill off any active pipes ! (so they can shut down ok)

	DrawThread* pThread = drawThreads;
	for(i = 0; i < nDrawThreads; i++, pThread++) {
		pThread->killPipe();
	}
	nDrawThreads = 0;

	// Clear the screen
	clear();

	// Check for window resize status
	if(resetStatus & RESET_RESIZE_BIT) {
		resetView();
	}

	// Reset the node states to empty
	nodes->reset();

	// Call any pipe-specific state resets, and get any recommended
	// pipesPerFrame counts
	this->reset();

	maxPipesPerFrame = NORMAL_PIPE_COUNT;

	// Set new number of drawing threads

	if(maxDrawThreads > 1) {
		// Set maximum # of pipes per frame
		maxPipesPerFrame = (int) (maxPipesPerFrame * 1.5);

		// Set # of draw threads
		nDrawThreads = MIN(maxPipesPerFrame, iRand2(2, maxDrawThreads));
	} else {
		nDrawThreads = 1;
	}
	nPipesDrawn = 0;

	// for now, either all NORMAL or all FLEX for each frame

	pThread = drawThreads;

	for(i = 0; i < nDrawThreads; i++, pThread++) {
		// Set up the modeling view

		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, glCfg.zTrans);

		// Rotate Scene
		glRotatef(glCfg.yRot, 0.0f, 1.0f, 0.0f);

		// create approppriate pipe for this thread slot
		pNewPipe = (Pipe*) new Pipe(this);
		pThread->setPipe(pNewPipe);
		pNewPipe->setChooseDirectionMethod(CHOOSE_DIR_RANDOM_WEIGHTED);

		// Launch the pipe (assumed: always more nodes than pipes starting, so
		// StartPipe cannot fail)
		// ! All pipe setup needs to be done before we call StartPipe, as this
		// is where the pipe starts drawing

		pThread->startPipe();

		nPipesDrawn++;
	}

	// Increment scene rotation for normal reset case
	if(resetStatus & RESET_BIT)
		incrementSceneRotation(&glCfg);

	// clear reset status
	resetStatus = 0;

#ifdef DO_TIMING
	Timer(TIMER_START);
#endif
}

void State::clear() {
	glClear(GL_DEPTH_BUFFER_BIT);
	// do a fast one-shot clear
	glClear(GL_COLOR_BUFFER_BIT);
}

void State::drawValidate() {
	if(!resetStatus)
		return;

	frameReset();
}

void State::draw(void* data) {
	int nKilledThreads = 0;
	bool bChooseNewLead = false;

	// Validate the draw state
	drawValidate();

	// Check each pipe's status
	DrawThread* pThread = drawThreads;

	for(int i = 0; i < nDrawThreads; i++, pThread++) {
		if(pThread->pPipe->isStuck()) {
			if(++nPipesDrawn > maxPipesPerFrame) {
				// Reaching pipe saturation - kill this pipe thread
				pThread->killPipe();
				nKilledThreads++;
			} else {
				// Start up another pipe
				if(!pThread->startPipe())
					// we won't be able to draw any more pipes this frame
					// (probably out of nodes)
					maxPipesPerFrame = nPipesDrawn;
			}
		}
	}

	// Whenever one or more pipes are killed, compact the thread list
	if(nKilledThreads) {
		compactThreadList();
		nDrawThreads -= nKilledThreads;
	}

	if(nDrawThreads == 0) {
		// This frame is finished - mark for reset on next Draw
		resetStatus |= RESET_BIT;
		return;
	}

	// Draw each pipe

	for(int i = 0; i < nDrawThreads; i++) {
		DrawThread* pThread = &drawThreads[i];
		pThread->drawPipe();
#ifdef DO_TIMING
		pipeCount++;
#endif
	}

	glFlush();
}

#define SWAP_SLOT(a, b) \
	DrawThread pTemp;   \
	pTemp = *(a);       \
	*(a) = *(b);        \
	*(b) = pTemp;

void State::compactThreadList() {
	if(nDrawThreads <= 1)
		// If only one active thread, it must be in slot 0 from previous
		// compactions - so nothing to do
		return;

	int iEmpty = 0;
	DrawThread* pThread = drawThreads;

	for(int i = 0; i < nDrawThreads; i++, pThread++) {
		if(pThread->pPipe) {
			if(iEmpty < i) {
				// swap active pipe thread and empty slot
				SWAP_SLOT(&(drawThreads[iEmpty]), pThread);
			}
			iEmpty++;
		}
	}
}

void State::finish(void* data) {
	delete(State*) data;
}

void State::buildObjects(float radius, float divSize, int nSlices) {
	ObjectBuildInfo* pBuildInfo = new ObjectBuildInfo;
	pBuildInfo->radius = radius;
	pBuildInfo->divSize = divSize;
	pBuildInfo->nSlices = nSlices;

	// Build pipes, elbows
	shortPipe = new PipeObject(pBuildInfo, divSize - 2 * radius);
	longPipe = new PipeObject(pBuildInfo, divSize);
	for(int i = 0; i < 4; i++) {
		elbows[i] = new ElbowObject(pBuildInfo, i);
		ballJoints[i] = NULL;
	}

	// Build just one ball joint when not texturing.  It is slightly
	// larger than standard ball joint, to prevent any pipe edges from
	// 'sticking' out of the ball.
	bigBall = new SphereObject(pBuildInfo, ROOT_TWO * radius / ((float) cos(PI / nSlices)));

	// build end cap
	ballCap = new SphereObject(pBuildInfo, ROOT_TWO * radius);
}

void State::reset() {
	// Set the joint style
	if(bCycleJointStyles) {
		if(++(jointStyle) >= NUM_JOINT_STYLES)
			jointStyle = 0;
	}
}

#define BLUE_MOON 153

int State::chooseJointType() {
	switch(jointStyle) {
		case ELBOWS:
			return ELBOW_JOINT;
		case BALLS:
			return BALL_JOINT;
		case EITHER:
			// draw a teapot once in a blue moon
			if(iRand(1000) == BLUE_MOON)
				return (TEAPOT);
		default:
			// otherwise an elbow or a ball (1/3 ball)
			if(!iRand(3))
				return BALL_JOINT;
			else
				return ELBOW_JOINT;
	}
}
