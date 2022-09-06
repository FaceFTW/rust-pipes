/**
 * @file pipe.h
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-06-29
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#pragma once
#include <vector>
#ifndef __PIPE_H_
#define __PIPE_H_

#include "utils.h"

namespace GlPipes {

typedef enum e_pipe_seg_type {
	PIPE_START,
	PIPE_SEG,
	PIPE_JOINT,
	PIPE_END,
	PIPE_INVALID
} PipeSegType;

typedef enum e_joint_type { JOINT_NORMAL, JOINT_BALL, JOINT_TEAPOT } JointType;

/*********************
DRAWING FUNCTIONS
**********************/
static void buildPipe(float length);
static void buildSphere();
static void buildElbow();

class PipePart {
public:
	PipePart(Point* pos, Direction dir, bool isEnd);                       //Assumes PIPE_START
	PipePart(Point* pos, Point* nextPos, Direction dir);                   //Assumes PIPE_SEG
	PipePart(Point* pos, Point* nextPos, Direction dir, Direction turnDir);//Assumes PIPE_JOINT

	//Getters
	Point* getPos();
	Point* getNextPos();
	Direction getDirection();
	Direction getTurnDirection();
	PipeSegType getType();

	//Setters
	void setJointType(JointType _type);

	//Other Methods
	// void setGlMaterial(MATERIAL* mat);
	void draw();

private:
	Point pos;
	Point nextPos;
	Direction dir;
	Direction turnDir;
	Direction notchVec;
	int weightStraight;
	PipeSegType segType;
	JointType jointType;
};

class Pipe {
public:
	Pipe(Point* startPos);
	~Pipe();

	void render();
	void addPipePart(PipePart* part);

private:
	int drawList;
	int numSlices;
	std::vector<PipePart>* pipeParts;
};

}// namespace GlPipes

#endif