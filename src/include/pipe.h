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

/******ENUM TYPEDEFS******/
typedef enum e_pipe_seg_type {
	PIPE_START,
	PIPE_SEG,
	PIPE_JOINT,
	PIPE_END,
	PIPE_INVALID
} PipeSegType;

typedef enum e_joint_type { JOINT_NORMAL, JOINT_BALL, JOINT_TEAPOT } JointType;

class PipePart {
public:
	PipePart(Point* _pos, Direction _dir, bool _isEnd);                        //Assumes PIPE_START
	PipePart(Point* _pos, Point* _nextPos, Direction _dir);                    //Assumes PIPE_SEG
	PipePart(Point* _pos, Point* _nextPos, Direction _dir, Direction _turnDir);//Assumes PIPE_JOINT
	~PipePart();

	//Getters
	Point* getPos();
	Point* getNextPos();
	Direction getDirection();
	Direction getTurnDirection();
	PipeSegType getType();

	//Setters
	void setJointType(JointType _type);

	//Other Methods
	void setGlMaterial(MATERIAL* mat);
	void draw();

protected:
	void drawSphereJoint();
	void drawPipeSegment();
	void drawPipeJoint();
	void drawSphere();
	void drawTeapot();

private:
	Point* pos;
	Point* nextPos;
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
	//TODO did i cause memory issues when forcing vector to use values????
	std::vector<PipePart>* pipeParts;
};

}// namespace GlPipes

#endif