/**
 * @file balljoint_object.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-22
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "../include/objects/balljoint_object.h"
#include "../include/structs.h"
#include "../include/utils.h"
#include <math.h>

BalljointObject::BalljointObject(ObjectBuildInfo* state, int notch, float start_s, float end_s) : Object() {
	build(state, notch, start_s, end_s);
}

void BalljointObject::build(ObjectBuildInfo* pBuildInfo, int notch,
                            float s_start, float s_end) {
	GLfloat ballRadius;
	GLfloat angle, delta_abuildrtAng, theta;
	GLint numPoints;
	GLfloat s_delta;
	POINT3D pi0[CACHE_SIZE];// 2 circles of untransformed points
	POINT3D pi1[CACHE_SIZE];
	POINT3D p0[CACHE_SIZE];// 2 rows of transformed points
	POINT3D p1[CACHE_SIZE];
	POINT3D n0[CACHE_SIZE];// 2 rows of normals
	POINT3D n1[CACHE_SIZE];
	float r[CACHE_SIZE];// radii of the circles
	POINT3D center;     // center of circle
	POINT3D anchor;     // where circle is anchored
	POINT3D *pA, *pB, *nA, *nB;
	int i, k;
	GLint stacks, slices;
	GLfloat radius = pBuildInfo->radius;

	slices = pBuildInfo->nSlices;
	stacks = slices;

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	// calculate the radii for each circle in the sweep, where
	// r[i] = y = sin(angle)/r

	angle = PI / 4;// first radius always at 45.0 degrees
	delta_a = (PI / 2.0f) / stacks;

	ballRadius = ROOT_TWO * radius;
	for(i = 0; i <= stacks; i++, angle += delta_a) {
		r[i] = (float) sin(angle) * ballRadius;
	}

	s_delta = s_end - s_start;

	numPoints = slices + 1;

	// unlike the elbow, the center for the ball joint is constant
	center.x = center.y = 0.0f;
	center.z = -radius;

	// starting angle along circle, increment 90.0 degrees each time
	startAng = notch * PI / 2;

	// calc initial circle of points for circle centered at 0,r,-r
	// points start at (0,r,0), and rotate circle CCW

	delta_a = 2 * PI / slices;
	for(i = 0, theta = startAng; i <= slices; i++, theta += delta_a) {
		pi0[i].x = r[0] * (float) sin(theta);
		pi0[i].y = radius;
		// translate z by -r, cuz these cos calcs are for circle at origin
		pi0[i].z = r[0] * (float) cos(theta) - r[0];
	}

	// anchor point
	anchor.x = anchor.z = 0.0f;
	anchor.y = radius;

	// calculate initial normals
	calcNormals(pi0, n0, &center, numPoints);

	// initial 's' texture coordinate
	// tex_s[0] = s_start;

	// setup pointers
	pA = pi0;// circles of transformed points
	pB = p0;
	nA = n0;// circles of transformed normals
	nB = n1;

	// now iterate throught the stacks

	glNewList(listNum, GL_COMPILE);

	for(i = 1; i <= stacks; i++) {
		// ! this angle must be negative, for correct vertex orientation !
		angle = -0.5f * PI * i / stacks;

		// calc the next circle of untransformed points into pi1[]

		for(k = 0, theta = startAng; k <= slices; k++, theta += delta_a) {
			pi1[k].x = r[i] * (float) sin(theta);
			pi1[k].y = radius;
			// translate z by -r, cuz calcs are for circle at origin
			pi1[k].z = r[i] * (float) cos(theta) - r[i];
		}

		// rotate cirle of points to next position
		transformCircle(angle, pi1, pB, numPoints, &anchor);

		// calculate normals
		calcNormals(pB, nB, &center, numPoints);

		// calculate next 's' texture coord
		// tex_s[1] = (GLfloat) s_start + s_delta * i / stacks;

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
