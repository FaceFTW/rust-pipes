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

#include <GL/gl.h>

class PIPE {
public:
	int type;
	IPOINT3D curPos;// current node position of pipe

	STATE* pState;// for state value access

	void SetChooseDirectionMethod(int method);
	void SetChooseStartPosMethod(int method);
	int ChooseNewDirection();
	bool IsStuck();// if pipe is stuck or not
	bool NowhereToRun() { return status == PIPE_OUT_OF_NODES; }
	// NORMAL_STATE* pNState;
	PIPE(STATE* state);

	//NORMAL_PIPE(STATE* state);
	void Start();
	GLint ChooseElbow(int oldDir, int newDir);
	void DrawJoint(int newDir);
	void Draw();//mf: could take param to draw n sections
	void DrawStartCap(int newDir);
	void DrawEndCap();

protected:
	//bool bTexture;
	float radius;// ideal radius (fluctuates for FPIPE)
	int status;  // ACTIVE/STUCK/STOPPED, etc.
	int lastDir; // last direction taken by pipe
	int notchVec;// current notch vector
	//PIPE(STATE* state);
	int weightStraight;// current weighting of going straight
	bool SetStartPos();// starting node position
	//void ChooseMaterial();
	void DrawTeapot();
	void UpdateCurrentPosition(int dir);
	void TranslateToCurrentPosition();

private:
	int chooseDirMethod;
	int chooseStartPosMethod;
	int GetBestDirsForChase(int* bestDirs);
};

extern void align_plusz(int newDir);
extern GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS];