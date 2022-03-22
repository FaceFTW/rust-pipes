/**
 * @file object.h
 * @author Alex "FaceFTW" Westerman
 * @brief Superclass for Various objects to draw
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __object_h_
#define __object_h_

#include <GL/gl.h>

typedef struct _ObjectBuildInfo {
	float radius;
	float divSize;
	int nSlices;
} ObjectBuildInfo;

class Object {
public:
	/**
 	* @brief Draw the object by calling its display list
 	*/
	void Draw() { glCallList(listNum); }

	/**
	 * @brief Construct a new OBJECT object
	 */
	Object() { listNum = glGenLists(1); }

	/**
	 * @brief Destroy the OBJECT object
	 * 
	 */
	~Object() { glDeleteLists(listNum, 1); }

	/**
	 * @brief Builds quadstrip between 2 rows of points.
	 *   Because the rotation has previously been defined CCW around the x-axis, using an A-B sequence will result in CCW quads 
	 * 
	 * @param pA points to one row of points
	 * @param pB points to the next rotated row.
	 * @param nA 
	 * @param nB 
	 * @param slices 
	 */
	static void makeQuadStrip(POINT3D* pA, POINT3D* pB, POINT3D* nA, POINT3D* nB, GLint slices) {
		GLint i;
		glBegin(GL_QUAD_STRIP);

		for(i = 0; i < slices; i++) {
			glNormal3fv((GLfloat*) nA++);
			glVertex3fv((GLfloat*) pA++);
			glNormal3fv((GLfloat*) nB++);
			glVertex3fv((GLfloat*) pB++);
		}

		glEnd();
	};

protected:
	int listNum;
	int nSlices;
};

#endif//__object_h_