/**
 * @file pipe_objects.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definitions of Drawing Objects
 * This was mostly copied from OBJECTS.h from the original source code
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __pipe_objects_h__
#define __pipe_objects_h__

// #include "sspipes.h"
#include "global_state.h"

class OBJECT_BUILD_INFO {
public:
	float radius;
	float divSize;
	int nSlices;
	//bool bTexture;
	//IPOINT2D* texRep;
};

class OBJECT {
public:
	/**
 	* @brief Draw the object by calling its display list
 	*/
	void Draw();

	OBJECT() { listNum = glGenLists(1); };
	~OBJECT() { glDeleteLists(listNum, 1); };

protected:
	int listNum;
	int nSlices;
};

class PIPE_OBJECT: public OBJECT {
public:
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length) { Build(state, length, 0.0f, 0.0f); }
	PIPE_OBJECT(OBJECT_BUILD_INFO* state, float length, float start_s, float end_s) { Build(state, length, start_s, end_s); }

private:
	void Build(OBJECT_BUILD_INFO* state, float length, float start_s, float s_end);
};

class ELBOW_OBJECT: public OBJECT {
public:
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch) { Build(state, notch, 0.0f, 0.0f); };
	ELBOW_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s) { Build(state, notch, start_s, end_s); }

private:
	/**
 	 * Builds elbows, by rotating a circle in the y=r plane centered at (0,r,-r), CW around the x-axis at anchor pt. (r = radius of the circle)                                    |
	 * @li rotation is 90.0 degrees, ending at circle in z=0 plane,      
	 *  centered at origin.                                           
	 * @li in order to 'mate' texture coords with the cylinders          
	 *  generated with glu, we generate 4 elbows, each corresponding  
	 *  to the 4 possible CW 90 degree orientations of the start point
	 *  for each circle.                                              
	 * @li We call this start point the 'notch'.  If we characterize 
	 * 	each notch by the axis it points down in the starting and 
	 *  ending circles of the elbow, then we get the following axis   
	 *  pairs for our 4 notches:                                      
	 *		@li +z,+y                                                 
	 *  	@li +x,+x                                                 
	 *  	@li -z,-y                                                 
	 *  	@li -x,-x                                                 
	 *  Since the start of the elbow always points down +y, the 4     
	 *  start notches give all possible 90.0 degree orientations      
	 *  around y-axis.                                                
	 * @li We can keep track of the current 'notch' vector to provide    
	 *  proper mating between primitives.                             
	 * @li Each circle of points is described CW from the start point,   
	 *  assuming looking down the +y axis(+y direction).              
	 * @li texture 's' starts at 0.0, and goes to 2.0*r/divSize at       
	 *  end of the elbow.  (Then a short pipe would start with this   
	 *  's', and run it to 1.0).     
 * 
 * @param state 
 * @param notch 
 * @param start_s 
 * @param end_s 
 */
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class BALLJOINT_OBJECT: public OBJECT {
public:
	// texturing version only
	BALLJOINT_OBJECT(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s) { Build(state, notch, start_s, end_s); }

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
	void Build(OBJECT_BUILD_INFO* state, int notch, float start_s, float end_s);
};

class SPHERE_OBJECT: public OBJECT {
public:
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s) { Build(state, radius, start_s, end_s); }
	SPHERE_OBJECT(OBJECT_BUILD_INFO* state, float radius) { Build(state, radius, 0.0f, 0.0f); };

private:
	void Build(OBJECT_BUILD_INFO* state, float radius, float start_s, float end_s);
};

#endif// __pipe_objects_h__
