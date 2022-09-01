/**
 * @file pipe_part.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief Implementation for the PipePart class
 * @version 0.1
 * @date 2022-07-05
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include "../include/pipe.h"
#include "../include/utils.h"
#include <GL/gl.h>

using namespace GlPipes;

PipePart::PipePart(Point* _pos, Direction _dir, bool _isEnd) {
	pos = _pos;
	dir = _dir;
	if(_isEnd) {
		segType = PIPE_END;
	} else {
		segType = PIPE_START;
	}

	nextPos = NULL;
	turnDir = DIR_NONE;
	notchVec = DIR_NONE;
	jointType = JOINT_NORMAL;
}

PipePart::PipePart(Point* _pos, Point* _nextPos, Direction _dir) {
	pos = _pos;
	dir = _dir;
	nextPos = _nextPos;
	segType = PIPE_SEG;

	turnDir = DIR_NONE;
	notchVec = DIR_NONE;
	jointType = JOINT_NORMAL;
}

PipePart::PipePart(Point* _pos, Point* _nextPos, Direction _dir, Direction _turnDir) {
	pos = _pos;
	dir = _dir;
	nextPos = _nextPos;
	segType = PIPE_JOINT;
	turnDir = _turnDir;

	notchVec = DIR_NONE;
	jointType = JOINT_NORMAL;
}

PipePart::~PipePart() {
	delete pos;
	delete nextPos;
}

Point* PipePart::getPos() { return pos; }
Point* PipePart::getNextPos() { return nextPos; }
Direction PipePart::getDirection() { return dir; }
Direction PipePart::getTurnDirection() { return turnDir; }
PipeSegType PipePart::getType() { return segType; }

void PipePart::setJointType(JointType _type) { jointType = _type; }

void PipePart::draw() {
	glPushMatrix();

	switch(segType) {
		case PIPE_START://TODO determine start/end code
		case PIPE_SEG: drawPipeSegment();
		case PIPE_JOINT:
			switch(jointType) {
				case JOINT_NORMAL: drawPipeJoint(); break;
				case JOINT_BALL: drawSphereJoint(); break;
				//TODO add teapot method
				case JOINT_TEAPOT: break;
			}
		case PIPE_END://TODO Determine start/end code
		case PIPE_INVALID: throw; break;
	}

	glPopMatrix();
}

