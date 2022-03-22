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
#include "include/utils.h"

// #include "dialog.h"
// #include "eval.h"
// #include "fpipe.h"
// #include "npipe.h"
// #include "pipe.h"
// #include "sspipes.h"
// #include "state.h"

STATE::STATE() {
	// various state values
	resetStatus = RESET_STARTUP_BIT;
	if(ulSurfStyle == SURFSTYLE_WIREFRAME) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Initialize GL state for the initial RC (sets texture state, so
	// (must come after LoadTextureFiles())

	initGLMisc();

	// set 'reference' radius value
	radius = 1.0f;

	// convert tesselation from fTesselFact(0.0-2.0) to tessLevel(0-MAX_TESS)
	//TODO where does this come from?
	int tessLevel = (int) (fTesselFact * (MAX_TESS + 1) / 2.0001f);
	nSlices = (tessLevel + 2) * 4;

	// Allocate basic NODE_ARRAY
	// NODE_ARRAY size is determined in Reshape (based on window size)
	nodes = new NODE_ARRAY;

	// Set drawing mode, and initialize accordingly.  For now, either all normal
	// or all flex pipes are drawn, but they could be combined later.
	// Can assume here that if there's any possibility that normal pipes
	// will be drawn, STATE will be initialized so that dlists are
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

	BuildObjects(pState->radius, pState->view.divSize, pState->nSlices,
	             pState->bTexture, &pState->texRep[0]);
}

STATE::~STATE() {
	if(nodes)
		delete nodes;

	DRAW_THREAD* pdt = &drawThreads[0];

	//From NORMAL_STATE
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

// void STATE::Repaint(LPRECT  void* data) {
// 	resetStatus |= RESET_REPAINT_BIT;
// }

// /**************************************************************************\
// * Reshape
// *   - called on resize, expose
// *   - always called on app startup
// *   - set new window size for VIEW object, and set resetStatus for validation
// *     at draw time
// *
// \**************************************************************************/

// void STATE::Reshape(int width, int height, void* data) {
// 	if(view.SetWinSize(width, height))
// 		resetStatus |= RESET_RESIZE_BIT;
//}

/**************************************************************************\
* ResetView
*
* Called on FrameReset resulting from change in viewing paramters (e.g. from
* a Resize event).
\**************************************************************************/

void STATE::ResetView() {
	IPOINT3D numNodes;

	// Have VIEW calculate the node array size based on view params
	calcNodeArraySize(&glCfg, &numNodes);

	// Resize the node array
	nodes->Resize(&numNodes);
}

/**************************************************************************\
* FrameReset
*
* Start a new frame of pipes
*
* The resetStatus parameter indicates what triggered the Reset.
*
\**************************************************************************/

static int PickRandomTexture(int i, int nTextures);

void STATE::FrameReset() {
	int i;
	float xRot, zRot;
	PIPE* pNewPipe;

#ifdef DO_TIMING
	Timer(TIMER_STOP);
#endif

	// DBGINFO("Pipes STATE::FrameReset:\n");

	// Kill off any active pipes ! (so they can shut down ok)

	DRAW_THREAD* pThread = drawThreads;
	for(i = 0; i < nDrawThreads; i++, pThread++) {
		pThread->KillPipe();
	}
	nDrawThreads = 0;

	// Clear the screen
	Clear();

	// Check for window resize status
	if(resetStatus & RESET_RESIZE_BIT) {
		ResetView();
	}

	// Reset the node states to empty
	nodes->Reset();

	// Call any pipe-specific state resets, and get any recommended
	// pipesPerFrame counts
	this->Reset();

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
		pNewPipe = (PIPE*) new PIPE(this);
		pThread->SetPipe(pNewPipe);
		pNewPipe->SetChooseDirectionMethod(CHOOSE_DIR_CHASE);

		// Launch the pipe (assumed: always more nodes than pipes starting, so
		// StartPipe cannot fail)
		// ! All pipe setup needs to be done before we call StartPipe, as this
		// is where the pipe starts drawing

		pThread->StartPipe();

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

/**************************************************************************\
* PickRandomTexture
*
* Pick a random texture index from a list.  Remove entry from list as it
* is picked.  Once all have been picked, or starting a new frame, reset.
*
* ! Routine not reentrant, should only be called by the main thread
* dispatcher (FrameReset)
\**************************************************************************/

static int
        PickRandomTexture(int iThread, int nTextures) {
	if(nTextures == 0)
		return 0;

	static int pickSet[MAX_TEXTURES] = {0};
	static int nPicked = 0;
	int i, index;

	if(iThread == 0)
		// new frame - force reset
		nPicked = nTextures;

	// reset condition
	if(++nPicked > nTextures) {
		for(i = 0; i < nTextures; i++) pickSet[i] = 0;
		nPicked = 1;// cuz
	}

	// Pick a random texture index
	index = iRand(nTextures);
	while(pickSet[index]) {
		// this index has alread been taken, try the next one
		if(++index >= nTextures)
			index = 0;
	}
	// Hopefully, the above loop will exit :).  This means that we have
	// found a texIndex that is available
	pickSet[index] = 1;// mark as taken
	return index;
}

void STATE::Clear() {
	glClear(GL_DEPTH_BUFFER_BIT);
	// do a fast one-shot clear
	glClear(GL_COLOR_BUFFER_BIT);
}

/**************************************************************************\
* DrawValidate
*
* Validation done before every Draw
*
* For now, this just involves checking resetStatus
*
\**************************************************************************/

void STATE::DrawValidate() {
	if(!resetStatus)
		return;

	FrameReset();
}

/**************************************************************************\
* Draw
*
* - Top-level pipe drawing routine
* - Each pipe thread keeps drawing new pipes until we reach maximum number
*   of pipes per frame - then each thread gets killed as soon as it gets
*   stuck.  Once number of drawing threads reaches 0, we start a new
*   frame
*
\**************************************************************************/

void STATE::Draw(void* data) {
	int nKilledThreads = 0;
	bool bChooseNewLead = false;

	// Validate the draw state

	DrawValidate();

	// Check each pipe's status

	DRAW_THREAD* pThread = drawThreads;

	for(int i = 0; i < nDrawThreads; i++, pThread++) {
		if(pThread->pPipe->IsStuck()) {
			if(++nPipesDrawn > maxPipesPerFrame) {
				// Reaching pipe saturation - kill this pipe thread
				pThread->KillPipe();
				nKilledThreads++;
			} else {
				// Start up another pipe
				if(!pThread->StartPipe())
					// we won't be able to draw any more pipes this frame
					// (probably out of nodes)
					maxPipesPerFrame = nPipesDrawn;
			}
		}
	}

	// Whenever one or more pipes are killed, compact the thread list
	if(nKilledThreads) {
		CompactThreadList();
		nDrawThreads -= nKilledThreads;
	}

	if(nDrawThreads == 0) {
		// This frame is finished - mark for reset on next Draw
		resetStatus |= RESET_BIT;
		return;
	}


	// Draw each pipe

	for(int i = 0, pThread = drawThreads[i]; i < nDrawThreads; i++, pThread++) {
		pThread->DrawPipe();
#ifdef DO_TIMING
		pipeCount++;
#endif
	}

	glFlush();
}

/**************************************************************************\
*
* CompactThreadList
*
* - Compact the thread list according to number of pipe threads killed
* - The pipes have been killed, but the RC's in each slot are still valid
*   and reusable.  So we swap up entries with valid pipes. This means that
*   the ordering of the RC's in the thread list will change during the life
*   of the program.  This should be OK.
*
\**************************************************************************/

#define SWAP_SLOT(a, b) \
	DRAW_THREAD pTemp;  \
	pTemp = *(a);       \
	*(a) = *(b);        \
	*(b) = pTemp;

void STATE::CompactThreadList() {
	if(nDrawThreads <= 1)
		// If only one active thread, it must be in slot 0 from previous
		// compactions - so nothing to do
		return;

	int iEmpty = 0;
	DRAW_THREAD* pThread = drawThreads;

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

/******************************Public*Routine******************************\
* Finish
*
* - Called when GL window being closed
*
\**************************************************************************/
void STATE::Finish(void* data) {
	delete(STATE*) data;
}

STATE::STATE(STATE* pState) {

}

/**************************************************************************\
* BuildObjects
*
* - Build all the pipe primitives
* - Different prims are built based on bTexture flag
*
\**************************************************************************/
void STATE::BuildObjects(float radius, float divSize, int nSlices,
                         bool bTexture, IPOINT2D* texRep) {
	OBJECT_BUILD_INFO* pBuildInfo = new OBJECT_BUILD_INFO;
	pBuildInfo->radius = radius;
	pBuildInfo->divSize = divSize;
	pBuildInfo->nSlices = nSlices;
	pBuildInfo->bTexture = bTexture;

	if(bTexture) {
		pBuildInfo->texRep = texRep;

		// Calc s texture intersection values
		float s_max = (float) texRep->y;
		float s_trans = s_max * 2.0f * radius / divSize;

		// Build short and long pipes
		shortPipe = new PIPE_OBJECT(pBuildInfo, divSize - 2 * radius,
		                            s_trans, s_max);
		longPipe = new PIPE_OBJECT(pBuildInfo, divSize, 0.0f, s_max);

		// Build elbow and ball joints
		for(int i = 0; i < 4; i++) {
			elbows[i] = new ELBOW_OBJECT(pBuildInfo, i, 0.0f, s_trans);
			ballJoints[i] = new BALLJOINT_OBJECT(pBuildInfo, i, 0.0f, s_trans);
		}

		bigBall = NULL;

		// Build end cap

		float s_start = -texRep->x * (ROOT_TWO - 1.0f) * radius / divSize;
		float s_end = texRep->x * (2.0f + (ROOT_TWO - 1.0f)) * radius / divSize;
		// calc compensation value, to prevent negative s coords
		float comp_s = (int) (-s_start) + 1.0f;
		s_start += comp_s;
		s_end += comp_s;
		ballCap = new SPHERE_OBJECT(pBuildInfo, ROOT_TWO * radius, s_start, s_end);

	} else {
		// Build pipes, elbows
		shortPipe = new PIPE_OBJECT(pBuildInfo, divSize - 2 * radius);
		longPipe = new PIPE_OBJECT(pBuildInfo, divSize);
		for(int i = 0; i < 4; i++) {
			elbows[i] = new ELBOW_OBJECT(pBuildInfo, i);
			ballJoints[i] = NULL;
		}

		// Build just one ball joint when not texturing.  It is slightly
		// larger than standard ball joint, to prevent any pipe edges from
		// 'sticking' out of the ball.
		bigBall = new SPHERE_OBJECT(pBuildInfo,
		                            ROOT_TWO * radius / ((float) cos(PI / nSlices)));

		// build end cap
		ballCap = new SPHERE_OBJECT(pBuildInfo, ROOT_TWO * radius);
	}
}

/**************************************************************************\
* Reset
*
* Reset frame attributes for normal pipes.
*
\**************************************************************************/

void STATE::Reset() {
	// Set the joint style
	if(bCycleJointStyles) {
		if(++(jointStyle) >= NUM_JOINT_STYLES)
			jointStyle = 0;
	}
}

/*-----------------------------------------------------------------------
|                                                                       |
|    ChooseJointType                                                    |
|       - Decides which type of joint to draw                           |
|                                                                       |
-----------------------------------------------------------------------*/

#define BLUE_MOON 153

int STATE::ChooseJointType() {
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
