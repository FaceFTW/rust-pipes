/**
 * @file elbow_object.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-22
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */
#include <math.h>

#include "../include/objects/elbow_object.h"
#include "../include/structs.h"
#include "../include/utils.h"

void ElbowObject::build(ObjectBuildInfo* pBuildInfo, int notch, float s_start, float s_end) {
	GLint stacks, slices;
	GLfloat angle, startAng;
	GLint numPoints;
	GLfloat s_delta;
	POINT3D pi[CACHE_SIZE];// initial row of points + center
	POINT3D p0[CACHE_SIZE];// 2 rows of points
	POINT3D p1[CACHE_SIZE];
	POINT3D n0[CACHE_SIZE];// 2 rows of normals
	POINT3D n1[CACHE_SIZE];
	POINT3D center;// center of circle
	POINT3D anchor;// where circle is anchored
	POINT3D *pA, *pB, *nA, *nB;
	int i;
	GLfloat radius = pBuildInfo->radius;

	slices = pBuildInfo->nSlices;
	stacks = slices / 2;

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	s_delta = s_end - s_start;

	numPoints = slices + 1;

	// starting angle increment 90.0 degrees each time
	startAng = notch * PI / 2;

	// calc initial circle of points for circle centered at 0,r,-r
	// points start at (0,r,0), and rotate circle CCW

	for(i = 0; i <= slices; i++) {
		angle = startAng + (2 * PI * i / slices);
		pi[i].x = radius * (float) sin(angle);
		pi[i].y = radius;
		// translate z by -r, cuz these cos calcs are for circle at origin
		pi[i].z = radius * (float) cos(angle) - radius;
	}

	// center point, tacked onto end of circle of points
	pi[i].x = 0.0f;
	pi[i].y = radius;
	pi[i].z = -radius;
	center = pi[i];

	// anchor point
	anchor.x = anchor.z = 0.0f;
	anchor.y = radius;

	// calculate initial normals
	calcNormals(pi, n0, &center, numPoints);

	// setup pointers
	pA = pi;
	pB = p0;
	nA = n0;
	nB = n1;

	// now iterate throught the stacks

	glNewList(listNum, GL_COMPILE);

	for(i = 1; i <= stacks; i++) {
		// ! this angle must be negative, for correct vertex orientation !
		angle = -0.5f * PI * i / stacks;

		// transform to get next circle of points + center
		transformCircle(angle, pi, pB, numPoints + 1, &anchor);

		// calculate normals
		center = pB[numPoints];
		calcNormals(pB, nB, &center, numPoints);

		// now we've got points and normals, ready to be quadstrip'd
		// MakeQuadStrip(pA, pB, nA, nB, bTexture, tex_s, tex_t, numPoints);
		makeQuadStrip(pA, pB, nA, nB, numPoints);

		// reset pointers
		pA = pB;
		nA = nB;
		pB = (pB == p0) ? p1 : p0;
		nB = (nB == n0) ? n1 : n0;
		// tex_s[0] = tex_s[1];
	}

	glEndList();
}