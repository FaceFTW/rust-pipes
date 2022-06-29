/**
 * @file balljoint_object.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __balljoint_object_h_
#define __balljoint_object_h_

#include "object.h"

class BalljointObject: public Object {
public:
	// texturing version only
	BalljointObject(ObjectBuildInfo* state, int notch, float start_s, float end_s);

private:
	/**
	 * @brief These are very similar to the elbows, in that the starting 
	 * and ending positions are almost identical.   The difference
	 * here is that the circles in the sweep describe a sphere as 
	 * they are rotated.     
	 * 
	 * @param state 
	 * @param notch 
	 * @param start_s 
	 * @param end_s 
	 */
	void build(ObjectBuildInfo* state, int notch, float start_s, float end_s);
};

#endif//__balljoint_object_h_