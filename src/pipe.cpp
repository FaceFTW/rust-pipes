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
#include "include/pipe_objects.h"
#include "include/utils.h"

#include <GL/glut.h>

Pipe::Pipe(State* state) {
	pState = state;
	radius = pState->radius;
	// default direction choosing is random
	chooseDirMethod = CHOOSE_DIR_RANDOM_WEIGHTED;
	chooseStartPosMethod = CHOOSE_STARTPOS_RANDOM;
	weightStraight = 1;
}

// Pipe::Pipe(STATE* pState)
//     : Pipe(pState) {

// 	// type = TYPE_NORMAL;
// 	// pState = pState->pState;

// 	// choose weighting of going straight
// 	if(!iRand(20))
// 		weightStraight = iRand2(MAX_WEIGHT_STRAIGHT / 4, MAX_WEIGHT_STRAIGHT);
// 	else
// 		weightStraight = 1 + iRand(4);
// }

void Pipe::chooseMaterial() {
	RandomTeaMaterial(true);
}

void Pipe::drawTeapot() {
	glFrontFace(GL_CW);
	glEnable(GL_NORMALIZE);
	glutSolidTeapot(2.5 * radius);
	glDisable(GL_NORMALIZE);
	glFrontFace(GL_CCW);
}

void Pipe::setChooseStartPosMethod(int method) {
	chooseStartPosMethod = method;
}

void Pipe::setChooseDirectionMethod(int method) {
	chooseDirMethod = method;
}

int Pipe::chooseNewDirection() {
	NODE_ARRAY* nodes = pState->nodes;
	int bestDirs[NUM_DIRS], nBestDirs;

	// figger out which fn to call
	switch(chooseDirMethod) {
		case CHOOSE_DIR_CHASE:
			if(nBestDirs = getBestDirsForChase(bestDirs))
				return nodes->choosePreferredDirection(&curPos, lastDir,
				                                       bestDirs, nBestDirs);
			// else lead pipe must have died, so fall thru:
		case CHOOSE_DIR_RANDOM_WEIGHTED:
		default:
			return nodes->chooseRandomDirection(&curPos, lastDir, weightStraight);
	}
}

int Pipe::getBestDirsForChase(int* bestDirs) {
	// Figure out best dirs to close in on leadPos

	//mf: will have to 'protect' leadPos with GetLeadPos() for multi-threading
	IPOINT3D* leadPos = &pState->pLeadPipe->curPos;
	IPOINT3D delta;
	int numDirs = 0;

	delta.x = leadPos->x - curPos.x;
	delta.y = leadPos->y - curPos.y;
	delta.z = leadPos->z - curPos.z;

	if(delta.x) {
		numDirs++;
		*bestDirs++ = delta.x > 0 ? PLUS_X : MINUS_X;
	}
	if(delta.y) {
		numDirs++;
		*bestDirs++ = delta.y > 0 ? PLUS_Y : MINUS_Y;
	}
	if(delta.z) {
		numDirs++;
		*bestDirs++ = delta.z > 0 ? PLUS_Z : MINUS_Z;
	}
	// It should be impossible for numDirs = 0 (all deltas = 0), as this
	// means curPos = leadPos
	return numDirs;
}

bool Pipe::setStartPos() {
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

bool Pipe::isStuck() {
	return status == Pipe_STUCK;
}

void Pipe::translateToCurrentPosition() {
	IPOINT3D numNodes;

	float divSize = pState->glCfg.divSize;
	// this requires knowing the size of the node array
	pState->nodes->GetNodeCount(&numNodes);
	glTranslatef((curPos.x - (numNodes.x - 1) / 2.0f) * divSize,
	             (curPos.y - (numNodes.y - 1) / 2.0f) * divSize,
	             (curPos.z - (numNodes.z - 1) / 2.0f) * divSize);
}

void Pipe::updateCurrentPosition(int newDir) {
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

void Pipe::start() {
	int newDir;

	// Set start position

	if(!SetStartPos()) {
		status = PIPE_OUT_OF_NODES;
		return;
	}

	// set a material
	chooseMaterial();

	// push matrix that has initial zTrans and rotation
	glPushMatrix();

	// Translate to current position
	translateToCurrentPosition();

	// Pick a random lastDir
	lastDir = iRand(NUM_DIRS);

	newDir = chooseNewDirection();

	if(newDir == DIR_NONE) {
		// pipe is stuck at the start node, draw something
		status = PIPE_STUCK;
		drawTeapot();
		glPopMatrix();
		return;
	} else
		status = PIPE_ACTIVE;

	// set initial notch vector
	notchVec = defCylNotch[newDir];

	drawStartCap(newDir);

	// move ahead 1.0*r to draw pipe
	glTranslatef(0.0f, 0.0f, radius);

	// draw short pipe
	align_notch(newDir, notchVec);
	pState->shortPipe->Draw();

	glPopMatrix();

	updateCurrentPosition(newDir);

	lastDir = newDir;
}

void Pipe::draw() {
	int newDir;

	newDir = chooseNewDirection();

	if(newDir == DIR_NONE) {// no empty nodes - nowhere to go
		drawEndCap();
		status = PIPE_STUCK;
		return;
	}

	// push matrix that has initial zTrans and rotation
	glPushMatrix();

	// Translate to current position
	translateToCurrentPosition();

	// draw joint if necessary, and pipe

	if(newDir != lastDir) {// turning! - we have to draw joint
		drawJoint(newDir);

		// draw short pipe
		align_notch(newDir, notchVec);
		pState->shortPipe->draw();
	} else {// no turn
		// draw long pipe, from point 1.0*r back
		align_plusz(newDir);
		align_notch(newDir, notchVec);
		glTranslatef(0.0f, 0.0f, -radius);
		pState->longPipe->draw();
	}

	glPopMatrix();

	updateCurrentPosition(newDir);
	lastDir = newDir;
}

void Pipe::drawStartCap(int newDir) {
	pState->bigBall->draw();
	align_plusz(newDir);
}

void Pipe::drawEndCap() {
	glPushMatrix();

	// Translate to current position
	translateToCurrentPosition();
	pState->bigBall->draw();

	glPopMatrix();
}

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

GLint Pipe::chooseElbow(int oldDir, int newDir) {
	int i;

	// precomputed table supplies correct elbow orientation
	for(i = 0; i < 4; i++) {
		if(notchElbDir[oldDir][newDir][i] == notchVec)
			return i;
	}
	// we shouldn't arrive here
	return -1;
}

void Pipe::drawJoint(int newDir) {
	int jointType;
	int iBend;

	jointType = pState->ChooseJointType();
#if PIPES_DEBUG
	if(newDir == oppositeDir[lastDir])
		printf("Warning: opposite dir chosen!\n");
#endif

	switch(jointType) {
		case BALL_JOINT:
			pState->bigBall->draw();
			align_plusz(newDir);

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
			iBend = chooseElbow(lastDir, newDir);
			if(iBend == -1) {
#if PipeS_DEBUG
				printf("ChooseElbow() screwed up\n");
#endif
				iBend = 0;// recover
			}
			pState->elbows[iBend]->draw();

			glPopMatrix();

			glTranslatef(0.0f, 0.0f, radius);
			break;

		default:
			// Horrors! It's the teapot!
			drawTeapot();
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
