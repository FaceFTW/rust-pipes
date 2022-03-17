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
	bool bTexture;
	IPOINT2D* texRep;
};

/**
 * @brief Display list objects
 *
 */
class OBJECT {
	protected:
	int         listNum;
	int         nSlices;
	public:
	/**
	 * @brief Draw the object by calling its display list
	 *
	 */
	void        Draw();

	/**
	 * @brief Construct a new OBJECT object
	 *
	 */
	OBJECT();

	/**
	 * @brief Destroy the OBJECT object
	 *
	 */
	~OBJECT();
};

class PIPE_OBJECT : public OBJECT {
	private:

	/**
	 * @brief
	 *
	 * @param state
	 * @param length
	 * @param start_s
	 * @param s_end
	 */
	void Build(OBJECT_BUILD_INFO* state, float length, float start_s, float s_end);
	public:

	/**
	 * @brief Construct a new pipe object object
	 *
	 * @param state
	 * @param length
	 */
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length);

	/**
	 * @brief Construct a new pipe object object
	 *
	 * @param state
	 * @param length
	 * @param start_s
	 * @param end_s
	 */
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length, float start_s, float end_s);
};

class ELBOW_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
	public:

	/**
	 * @brief Construct a new elbow object
	 * 
	 * @param state 
	 * @param notch 
	 */
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch);
	
	/**
	 * @brief Construct a new elbow object
	 * 
	 * @param state 
	 * @param notch 
	 * @param start_s 
	 * @param end_s 
	 */
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class BALLJOINT_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
	public:
		
	/**
	 * @brief Construct a new balljoint object. Texturing version only
	 * 
	 * @param state 
	 * @param notch 
	 * @param start_s 
	 * @param end_s 
	 */
	BALLJOINT_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class SPHERE_OBJECT : public OBJECT {
	private:
	void Build(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s);
	public:

	/**
	 * @brief Construct a new sphere object object
	 * 
	 * @param state 
	 * @param radius 
	 * @param start_s 
	 * @param end_s 
	 */
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s);
	
	/**
	 * @brief Construct a new sphere object object
	 * 
	 * @param state 
	 * @param radius 
	 */
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius);
};

#endif // __pipe_objects_h__
