/**
 * @file elbow_object.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "object.h"

#ifndef __elbow_object_h_
#define __elbow_object_h_

class ElbowObject: public Object {
public:
	ElbowObject(ObjectBuildInfo* state, int notch) { build(state, notch, 0.0f, 0.0f); };
	ElbowObject(ObjectBuildInfo* state, int notch, float start_s, float end_s) { build(state, notch, start_s, end_s); }

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
	void build(ObjectBuildInfo* state, int notch, float start_s, float end_s);
};

#endif//__elbow_object_h_