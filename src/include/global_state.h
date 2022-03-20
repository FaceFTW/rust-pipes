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
#include "node_array.h"
#include "pipe.h"

// Reset status

#define RESET_STARTUP_BIT (1L << 0)
#define RESET_NORMAL_BIT (1L << 1)
#define RESET_RESIZE_BIT (1L << 2)
#define RESET_REPAINT_BIT (1L << 3)

// Frame draw schemes

class DRAW_THREAD {
private:
	//    HDC         hdc;
	//    HTEXTURE    htex;

public:
	//HGLRC hglrc;// rc to draw with (public so STATE can delete)
	int priority;

	DRAW_THREAD();
	~DRAW_THREAD();
	PIPE* pPipe;// generic pipe ptr
	//void SetRCDC(HGLRC rc, HDC hdc);
	//bool HasRC();
	//HGLRC GetRC();
	//void MakeRCCurrent();
	//void        SetTexture( HTEXTURE htex );
	void SetPipe(PIPE* pipe);
	bool StartPipe();
	void DrawPipe();
	void KillPipe();
};


// this used for traditional pipe drawing

class PIPE_OBJECT;
class ELBOW_OBJECT;
class SPHERE_OBJECT;
class BALLJOINT_OBJECT;

// // Program existence instance

// class NORMAL_STATE;
// class FLEX_STATE;

class STATE {
public:
	//HGLRC       shareRC;        // RC that objects are shared from

	//PIPE* pLeadPipe;// lead pipe for chase scenarios

	int nSlices;// reference # of slices around a pipe
	//BOOL        bTexture;       // global texture enable
	int nTextures;
	//TEXTURE     texture[MAX_TEXTURES];
	//IPOINT2D    texRep[MAX_TEXTURES];

	//VIEW        view;           // viewing parameters
	float radius;     // 'reference' pipe radius value
	NODE_ARRAY* nodes;// for keeping track of draw space
	// NORMAL_STATE *pNState;
	// FLEX_STATE  *pFState;

	//STATE(BOOL bFlexMode, BOOL bMultiPipes);
	STATE();
	~STATE();
	void Reshape(int width, int height, void* data);
	void Repaint(LPRECT pRect, void* data);
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
	//int         drawMode;       // drawing mode (flex or normal for now)
	//int         drawScheme;     // random or chase

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
	void Clear();
	void ChooseNewLeadPipe();
	void CompactThreadList();
	//BOOL        LoadTextureFiles();
	//BOOL        LoadTextureFiles( TEXFILE *pTexFile, int nTexFiles,
	//TEX_RES *pTexRes );
	//void        CalcTexRepFactors();
	int CalcMaxPipesPerFrame();
};

#endif// __state_h__
