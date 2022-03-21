/**
 * @file pipe.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "include/pipe.h"
#include "include/graphics/gl_materials.h"
#include "include/utils.h"
#include "include/pipe_objects.h"

#include <GL/glut.h>

PIPE::PIPE(STATE* state) {
	pState = state;
	//bTexture = pState->bTexture;
	radius = pState->radius;
	// default direction choosing is random
	chooseDirMethod = CHOOSE_DIR_RANDOM_WEIGHTED;
	chooseStartPosMethod = CHOOSE_STARTPOS_RANDOM;
	weightStraight = 1;
}

void PIPE::ChooseMaterial() {
	// if(bTexture)
	// 	RandomTexMaterial(true);
	// else
	RandomTeaMaterial(true);
}

/**************************************************************************\
*
* DrawTeapot
*
\**************************************************************************/

// extern void ResetEvaluator( bool bTexture );

void PIPE::DrawTeapot() {
	glFrontFace(GL_CW);
	glEnable(GL_NORMALIZE);
	glutSolidTeapot(2.5 * radius);
	glDisable(GL_NORMALIZE);
	glFrontFace(GL_CCW);
	// if( type != TYPE_NORMAL ) {
	//     // Re-init flex's evaluator state (teapot uses evaluators as well,
	//     //  and messes up the state).
	//     ResetEvaluator( bTexture );
	// }
}

/******************************Public*Routine******************************\
* SetChooseDirectionMethod
\**************************************************************************/

void PIPE::SetChooseDirectionMethod(int method) {
	chooseDirMethod = method;
}

/**************************************************************************\
*
* ChooseNewDirection
*
* Call direction-finding function based on current method
* This is a generic entry point that is used by some pipe types
*
\**************************************************************************/

int PIPE::ChooseNewDirection() {
	NODE_ARRAY* nodes = pState->nodes;
	int bestDirs[NUM_DIRS], nBestDirs;

	// figger out which fn to call
	switch(chooseDirMethod) {
		case CHOOSE_DIR_CHASE:
			if(nBestDirs = GetBestDirsForChase(bestDirs))
				return nodes->ChoosePreferredDirection(&curPos, lastDir,
				                                       bestDirs, nBestDirs);
			// else lead pipe must have died, so fall thru:
		case CHOOSE_DIR_RANDOM_WEIGHTED:
		default:
			return nodes->ChooseRandomDirection(&curPos, lastDir, weightStraight);
	}
}

/**************************************************************************\
*
* GetBestDirsForChase
*
* Find the best directions to take to close in on the lead pipe in chase mode.
*
\**************************************************************************/

// int PIPE::GetBestDirsForChase(int* bestDirs) {
// 	// Figure out best dirs to close in on leadPos

// 	//mf: will have to 'protect' leadPos with GetLeadPos() for multi-threading
// 	IPOINT3D* leadPos = &pState->pLeadPipe->curPos;
// 	IPOINT3D delta;
// 	int numDirs = 0;

// 	delta.x = leadPos->x - curPos.x;
// 	delta.y = leadPos->y - curPos.y;
// 	delta.z = leadPos->z - curPos.z;

// 	if(delta.x) {
// 		numDirs++;
// 		*bestDirs++ = delta.x > 0 ? PLUS_X : MINUS_X;
// 	}
// 	if(delta.y) {
// 		numDirs++;
// 		*bestDirs++ = delta.y > 0 ? PLUS_Y : MINUS_Y;
// 	}
// 	if(delta.z) {
// 		numDirs++;
// 		*bestDirs++ = delta.z > 0 ? PLUS_Z : MINUS_Z;
// 	}
// 	// It should be impossible for numDirs = 0 (all deltas = 0), as this
// 	// means curPos = leadPos
// 	return numDirs;
// }

/******************************Public*Routine******************************\
* SetChooseStartPosMethod
\**************************************************************************/

void PIPE::SetChooseStartPosMethod(int method) {
	chooseStartPosMethod = method;
}

/******************************Public*Routine******************************\
* PIPE::SetStartPos
*
* - Find an empty node to start the pipe on
*
\**************************************************************************/

bool PIPE::SetStartPos() {
	NODE_ARRAY* nodes = pState->nodes;

	switch(chooseStartPosMethod) {

		case CHOOSE_STARTPOS_RANDOM:
		default:
			if(!nodes->FindRandomEmptyNode(&curPos)) {
				return false;
			}
			return true;

		case CHOOSE_STARTPOS_FURTHEST:
			// find node furthest away from curPos
			IPOINT3D refPos, numNodes;
			nodes->GetNodeCount(&numNodes);
			refPos.x = (curPos.x >= (numNodes.x / 2)) ? 0 : numNodes.x - 1;
			refPos.y = (curPos.y >= (numNodes.y / 2)) ? 0 : numNodes.y - 1;
			refPos.z = (curPos.z >= (numNodes.z / 2)) ? 0 : numNodes.z - 1;

			if(!nodes->TakeClosestEmptyNode(&curPos, &refPos)) {
				return false;
			}
			return true;
	}
}

/******************************Public*Routine******************************\
* PIPE::IsStuck
\**************************************************************************/

bool PIPE::IsStuck() {
	return status == PIPE_STUCK;
}

/******************************Public*Routine******************************\
* PIPE::TranslateToCurrentPosition
*
\**************************************************************************/

void PIPE::TranslateToCurrentPosition() {
	IPOINT3D numNodes;

	float divSize = pState->glCfg.divSize;
	// this requires knowing the size of the node array
	pState->nodes->GetNodeCount(&numNodes);
	glTranslatef((curPos.x - (numNodes.x - 1) / 2.0f) * divSize,
	             (curPos.y - (numNodes.y - 1) / 2.0f) * divSize,
	             (curPos.z - (numNodes.z - 1) / 2.0f) * divSize);
}

/**************************************************************************\
*
* UpdateCurrentPosition
* 
* Increment current position according to direction taken
\**************************************************************************/

void PIPE::UpdateCurrentPosition(int newDir) {
	switch(newDir) {
		case PLUS_X:
			curPos.x += 1;
			break;
		case MINUS_X:
			curPos.x -= 1;
			break;
		case PLUS_Y:
			curPos.y += 1;
			break;
		case MINUS_Y:
			curPos.y -= 1;
			break;
		case PLUS_Z:
			curPos.z += 1;
			break;
		case MINUS_Z:
			curPos.z -= 1;
			break;
	}
}

/******************************Public*Routine******************************\
* align_plusz
*
* - Aligns the z axis along specified direction
* - Used for all types of pipes
*
\**************************************************************************/

void align_plusz(int newDir) {
	// align +z along new direction
	switch(newDir) {
		case PLUS_X:
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			break;
		case MINUS_X:
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			break;
		case PLUS_Y:
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			break;
		case MINUS_Y:
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			break;
		case PLUS_Z:
			glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
			break;
		case MINUS_Z:
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			break;
	}
}

/**************************************************************************\
* this array tells you which way the notch will be once you make
* a turn
* format: notchTurn[oldDir][newDir][notchVec] 
*
\**************************************************************************/

GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS] = {
        // oldDir = +x
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, MINUS_X, PLUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_X, MINUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_Y, MINUS_Y, MINUS_X, PLUS_X,
        iXX, iXX, PLUS_Y, MINUS_Y, PLUS_X, MINUS_X,
        // oldDir = -x
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, PLUS_X, MINUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, MINUS_X, PLUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_Y, MINUS_Y, PLUS_X, MINUS_X,
        iXX, iXX, PLUS_Y, MINUS_Y, MINUS_X, PLUS_X,
        // oldDir = +y
        MINUS_Y, PLUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        PLUS_Y, MINUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        PLUS_X, MINUS_X, iXX, iXX, MINUS_Y, PLUS_Y,
        PLUS_X, MINUS_X, iXX, iXX, PLUS_Y, MINUS_Y,
        // oldDir = -y
        PLUS_Y, MINUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        MINUS_Y, PLUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        PLUS_X, MINUS_X, iXX, iXX, PLUS_Y, MINUS_Y,
        PLUS_X, MINUS_X, iXX, iXX, MINUS_Y, PLUS_Y,
        // oldDir = +z
        MINUS_Z, PLUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_Z, MINUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_X, MINUS_X, MINUS_Z, PLUS_Z, iXX, iXX,
        PLUS_X, MINUS_X, PLUS_Z, MINUS_Z, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        // oldDir = -z
        PLUS_Z, MINUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        MINUS_Z, PLUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_X, MINUS_X, PLUS_Z, MINUS_Z, iXX, iXX,
        PLUS_X, MINUS_X, MINUS_Z, PLUS_Z, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX};

static GLint defCylNotch[NUM_DIRS] =
        {PLUS_Y, PLUS_Y, MINUS_Z, PLUS_Z, PLUS_Y, PLUS_Y};


// PIPE::PIPE(STATE* pState)
//     : PIPE(pState) {

// 	// type = TYPE_NORMAL;
// 	// pState = pState->pState;

// 	// choose weighting of going straight
// 	if(!iRand(20))
// 		weightStraight = iRand2(MAX_WEIGHT_STRAIGHT / 4, MAX_WEIGHT_STRAIGHT);
// 	else
// 		weightStraight = 1 + iRand(4);
// }

/**************************************************************************\
* Start
*
* Start drawing a new normal pipe
*
* - Draw a start cap and short pipe in new direction
*
\**************************************************************************/

void PIPE::Start() {
	int newDir;

	// Set start position

	if(!SetStartPos()) {
		status = PIPE_OUT_OF_NODES;
		return;
	}

	// set a material

	ChooseMaterial();

	// push matrix that has initial zTrans and rotation
	glPushMatrix();

	// Translate to current position
	TranslateToCurrentPosition();

	// Pick a random lastDir
	lastDir = iRand(NUM_DIRS);

	newDir = ChooseNewDirection();

	if(newDir == DIR_NONE) {
		// pipe is stuck at the start node, draw something
		status = PIPE_STUCK;
		DrawTeapot();
		glPopMatrix();
		return;
	} else
		status = PIPE_ACTIVE;

	// set initial notch vector
	notchVec = defCylNotch[newDir];

	DrawStartCap(newDir);

	// move ahead 1.0*r to draw pipe
	glTranslatef(0.0f, 0.0f, radius);

	// draw short pipe
	align_notch(newDir, notchVec);
	pState->shortPipe->Draw();

	glPopMatrix();

	UpdateCurrentPosition(newDir);

	lastDir = newDir;
}

/**************************************************************************\
* Draw
*
* - if turning, draws a joint and a short cylinder, otherwise
*   draws a long cylinder.
* - the 'current node' is set as the one we draw thru the NEXT
*   time around.
*
\**************************************************************************/

void PIPE::Draw() {
	int newDir;

	newDir = ChooseNewDirection();

	if(newDir == DIR_NONE) {// no empty nodes - nowhere to go
		DrawEndCap();
		status = PIPE_STUCK;
		return;
	}

	// push matrix that has initial zTrans and rotation
	glPushMatrix();

	// Translate to current position
	TranslateToCurrentPosition();

	// draw joint if necessary, and pipe

	if(newDir != lastDir) {// turning! - we have to draw joint
		DrawJoint(newDir);

		// draw short pipe
		align_notch(newDir, notchVec);
		pState->shortPipe->Draw();
	} else {// no turn
		// draw long pipe, from point 1.0*r back
		align_plusz(newDir);
		align_notch(newDir, notchVec);
		glTranslatef(0.0f, 0.0f, -radius);
		pState->longPipe->Draw();
	}

	glPopMatrix();

	UpdateCurrentPosition(newDir);

	lastDir = newDir;
}

/**************************************************************************\
* DrawStartCap
*
* Cap the start of the pipe with a ball
*
\**************************************************************************/

void PIPE::DrawStartCap(int newDir) {
	// if(bTexture) {
	// 	align_plusz(newDir);
	// 	pState->ballCap->Draw();
	// } else {
	// draw big ball in default orientation
	pState->bigBall->Draw();
	align_plusz(newDir);
}


/**************************************************************************\
* DrawEndCap():
*
* - Draws a ball, used to cap end of a pipe
*
\**************************************************************************/

void PIPE::DrawEndCap() {
	glPushMatrix();

	// Translate to current position
	TranslateToCurrentPosition();

	// if(bTexture) {
	// 	glPushMatrix();
	// 	align_plusz(lastDir);
	// 	align_notch(lastDir, notchVec);
	// 	pState->ballCap->Draw();
	// 	glPopMatrix();
	// } else
	pState->bigBall->Draw();

	glPopMatrix();
}

/**************************************************************************\
* ChooseElbow
*
* - Decides which elbow to draw
* - The beginning of each elbow is aligned along +y, and we have
*   to choose the one with the notch in correct position
* - The 'primary' start notch (elbow[0]) is in same direction as
*   newDir, and successive elbows rotate this notch CCW around +y
*
\**************************************************************************/

// this array supplies the sequence of elbow notch vectors, given
//  oldDir and newDir  (0's are don't cares)
// it is also used to determine the ending notch of an elbow
static GLint notchElbDir[NUM_DIRS][NUM_DIRS][4] = {
        // oldDir = +x
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX,
        PLUS_Y, MINUS_Z, MINUS_Y, PLUS_Z,
        MINUS_Y, PLUS_Z, PLUS_Y, MINUS_Z,
        PLUS_Z, PLUS_Y, MINUS_Z, MINUS_Y,
        MINUS_Z, MINUS_Y, PLUS_Z, PLUS_Y,
        // oldDir = -x
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX,
        PLUS_Y, PLUS_Z, MINUS_Y, MINUS_Z,
        MINUS_Y, MINUS_Z, PLUS_Y, PLUS_Z,
        PLUS_Z, MINUS_Y, MINUS_Z, PLUS_Y,
        MINUS_Z, PLUS_Y, PLUS_Z, MINUS_Y,
        // oldDir = +y
        PLUS_X, PLUS_Z, MINUS_X, MINUS_Z,
        MINUS_X, MINUS_Z, PLUS_X, PLUS_Z,
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX,
        PLUS_Z, MINUS_X, MINUS_Z, PLUS_X,
        MINUS_Z, PLUS_X, PLUS_Z, MINUS_X,
        // oldDir = -y
        PLUS_X, MINUS_Z, MINUS_X, PLUS_Z,
        MINUS_X, PLUS_Z, PLUS_X, MINUS_Z,
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX,
        PLUS_Z, PLUS_X, MINUS_Z, MINUS_X,
        MINUS_Z, MINUS_X, PLUS_Z, PLUS_X,
        // oldDir = +z
        PLUS_X, MINUS_Y, MINUS_X, PLUS_Y,
        MINUS_X, PLUS_Y, PLUS_X, MINUS_Y,
        PLUS_Y, PLUS_X, MINUS_Y, MINUS_X,
        MINUS_Y, MINUS_X, PLUS_Y, PLUS_X,
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX,
        // oldDir = -z
        PLUS_X, PLUS_Y, MINUS_X, MINUS_Y,
        MINUS_X, MINUS_Y, PLUS_X, PLUS_Y,
        PLUS_Y, MINUS_X, MINUS_Y, PLUS_X,
        MINUS_Y, PLUS_X, PLUS_Y, MINUS_X,
        iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX};

GLint PIPE::ChooseElbow(int oldDir, int newDir) {
	int i;

	// precomputed table supplies correct elbow orientation
	for(i = 0; i < 4; i++) {
		if(notchElbDir[oldDir][newDir][i] == notchVec)
			return i;
	}
	// we shouldn't arrive here
	return -1;
}

/**************************************************************************\
* DrawJoint
*
* Draw a joint between 2 pipes
*
\**************************************************************************/

void PIPE::DrawJoint(int newDir) {
	int jointType;
	int iBend;

	jointType = pState->ChooseJointType();
#if PIPES_DEBUG
	if(newDir == oppositeDir[lastDir])
		printf("Warning: opposite dir chosen!\n");
#endif

	switch(jointType) {
		case BALL_JOINT:
			// if(bTexture) {
			// 	// use special texture-friendly ballJoints

			// 	align_plusz(newDir);
			// 	glPushMatrix();

			// 	align_plusy(lastDir, newDir);

			// 	// translate forward 1.0*r along +z to get set for drawing elbow
			// 	glTranslatef(0.0f, 0.0f, radius);
			// 	// decide which elbow orientation to use
			// 	iBend = ChooseElbow(lastDir, newDir);
			// 	pState->ballJoints[iBend]->Draw();

			// 	glPopMatrix();
			// } else {
			// draw big ball in default orientation
			pState->bigBall->Draw();
			align_plusz(newDir);
			// }
			// move ahead 1.0*r to draw pipe
			glTranslatef(0.0f, 0.0f, radius);
			break;

		case ELBOW_JOINT:
			align_plusz(newDir);

			// the align_plusy() here will screw up our notch calcs, so
			//  we push-pop

			glPushMatrix();

			align_plusy(lastDir, newDir);

			// translate forward 1.0*r along +z to get set for drawing elbow
			glTranslatef(0.0f, 0.0f, radius);
			// decide which elbow orientation to use
			iBend = ChooseElbow(lastDir, newDir);
			if(iBend == -1) {
#if PIPES_DEBUG
				printf("ChooseElbow() screwed up\n");
#endif
				iBend = 0;// recover
			}
			pState->elbows[iBend]->Draw();

			glPopMatrix();

			glTranslatef(0.0f, 0.0f, radius);
			break;

		default:
			// Horrors! It's the teapot!
			DrawTeapot();
			align_plusz(newDir);
			// move ahead 1.0*r to draw pipe
			glTranslatef(0.0f, 0.0f, radius);
	}

	// update the current notch vector
	notchVec = notchTurn[lastDir][newDir][notchVec];
#if PIPES_DEBUG
	if(notchVec == iXX)
		printf("notchTurn gave bad value\n");
#endif
}

/**************************************************************************\
* Geometry functions
\**************************************************************************/

static float RotZ[NUM_DIRS][NUM_DIRS] = {
        0.0f, 0.0f, 90.0f, 90.0f, 90.0f, -90.0f,
        0.0f, 0.0f, -90.0f, -90.0f, -90.0f, 90.0f,
        180.0f, 180.0f, 0.0f, 0.0f, 180.0f, 180.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -90.0f, 90.0f, 0.0f, 180.0f, 0.0f, 0.0f,
        90.0f, -90.0f, 180.0f, 0.0f, 0.0f, 0.0f};

/*-----------------------------------------------------------------------
|                                                                       |
|    align_plusy( int lastDir, int newDir )                             |
|       - Assuming +z axis is already aligned with newDir, align        |
|         +y axis BACK along lastDir                                    |
|                                                                       |
-----------------------------------------------------------------------*/

static void
        align_plusy(int oldDir, int newDir) {
	GLfloat rotz;

	rotz = RotZ[oldDir][newDir];
	glRotatef(rotz, 0.0f, 0.0f, 1.0f);
}

// given a dir, determine how much to rotate cylinder around z to match notches
// format is [newDir][notchVec]

static GLfloat alignNotchRot[NUM_DIRS][NUM_DIRS] = {
        fXX, fXX, 0.0f, 180.0f, 90.0f, -90.0f,
        fXX, fXX, 0.0f, 180.0f, -90.0f, 90.0f,
        -90.0f, 90.0f, fXX, fXX, 180.0f, 0.0f,
        -90.0f, 90.0f, fXX, fXX, 0.0f, 180.0f,
        -90.0f, 90.0f, 0.0f, 180.0f, fXX, fXX,
        90.0f, -90.0f, 0.0f, 180.0f, fXX, fXX};

/*-----------------------------------------------------------------------
|                                                                       |
|    align_notch( int newDir )                                          |
|       - a cylinder is notched, and we have to line this up            |
|         with the previous primitive's notch which is maintained as    |
|         notchVec.                                                     |
|       - this adds a rotation around z to achieve this                 |
|                                                                       |
-----------------------------------------------------------------------*/

static void
        align_notch(int newDir, int notch) {
	GLfloat rotz;
	GLint curNotch;

	// figure out where notch is presently after +z alignment
	curNotch = defCylNotch[newDir];
	// (don't need this now we have lut)

	// look up rotation value in table
	rotz = alignNotchRot[newDir][notch];
#if PIPES_DEBUG
	if(rotz == fXX) {
		printf("align_notch(): unexpected value\n");
		return;
	}
#endif

	if(rotz != 0.0f)
		glRotatef(rotz, 0.0f, 0.0f, 1.0f);
}
