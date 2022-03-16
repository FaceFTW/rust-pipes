/**
 * @file objects.h
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

//NOTE the code here is mostly a copy of OBJECTS.H, will change over time

#ifndef __objects_h__
#define __objects_h__

#include "pipe_main.h"
#include "state/state.h"

class OBJECT_BUILD_INFO {
	public:
	float radius;
	float divSize;
	int   nSlices;
	BOOL  bTexture;
	IPOINT2D* texRep;
};

/**************************************************************************\
*
* OBJECT classes
*
* - Display list objects
*
\**************************************************************************/

class OBJECT {
	protected:
	int         listNum;
	int         nSlices;
	public:
	void        Draw();

	OBJECT();
	~OBJECT();
};

class PIPE_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, float length, float start_s, float s_end);
	public:
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length);
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length, float start_s, float end_s);
};

class ELBOW_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
	public:
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch);
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class BALLJOINT_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
	public:
		// texturing version only
	BALLJOINT_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class SPHERE_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s);
	public:
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s);
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius);
};

#endif // __pipe_objects_h__
