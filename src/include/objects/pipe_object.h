/**
 * @file pipe_object.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "../structs.h"
#include "object.h"

#ifndef __pipe_object_h_
#define __pipe_object_h_

class PipeObject: public Object {
public:
	PipeObject(ObjectBuildInfo* state, float length);
	PipeObject(ObjectBuildInfo* state, float length, float start_s, float end_s);

private:
	void build(ObjectBuildInfo* state, float length, float start_s, float s_end);
};

#endif//__pipe_object_h_