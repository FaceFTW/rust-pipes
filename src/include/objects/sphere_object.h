/**
 * @file sphere_object.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "object.h"

class SphereObject: public Object {
public:
	SphereObject(ObjectBuildInfo* state, float radius, float start_s, float end_s);
	SphereObject(ObjectBuildInfo* state, float radius);

private:
	void build(ObjectBuildInfo* state, float radius, float start_s, float end_s);
};