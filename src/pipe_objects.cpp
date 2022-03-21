/**
 * @file pipe_objects.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief Implementations of drawing objects.
 * This was mainly copied from OBJECTS.CPP
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "include/pipe_objects.h"
#include "include/constants.h"
#include "include/utils.h"
//#include "sscommon.h"
//#include "sspipes.h"
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>

void OBJECT::Draw() {
	glCallList(listNum);
}

/*----------------------------------------------------------------------\
|    MakeQuadStrip()                                                    |
|       - builds quadstrip between 2 rows of points. pA points to one   |
|         row of points, and pB to the next rotated row.  Because       |
|         the rotation has previously been defined CCW around the       |
|         x-axis, using an A-B sequence will result in CCW quads        |
|                                                                       |
\----------------------------------------------------------------------*/
static void MakeQuadStrip(
        POINT3D* pA,
        POINT3D* pB,
        POINT3D* nA,
        POINT3D* nB,
        // BOOL bTexture,
        // GLfloat* tex_s,
        // GLfloat* tex_t,
        GLint slices) {
	GLint i;

	glBegin(GL_QUAD_STRIP);

	for(i = 0; i < slices; i++) {
		glNormal3fv((GLfloat*) nA++);
		// if(bTexture)
		// 	glTexCoord2f(tex_s[0], *tex_t);
		glVertex3fv((GLfloat*) pA++);
		glNormal3fv((GLfloat*) nB++);
		// if(bTexture)
		// 	glTexCoord2f(tex_s[1], *tex_t++);
		glVertex3fv((GLfloat*) pB++);
	}

	glEnd();
}

void ELBOW_OBJECT::Build(OBJECT_BUILD_INFO* pBuildInfo, int notch, float s_start, float s_end) {
	GLint stacks, slices;
	GLfloat angle, startAng;
	GLint numPoints;
	GLfloat s_delta;
	POINT3D pi[CACHE_SIZE];// initial row of points + center
	POINT3D p0[CACHE_SIZE];// 2 rows of points
	POINT3D p1[CACHE_SIZE];
	POINT3D n0[CACHE_SIZE];// 2 rows of normals
	POINT3D n1[CACHE_SIZE];
	// GLfloat tex_t[CACHE_SIZE];// 't' texture coords
	// GLfloat tex_s[2];         // 's' texture coords
	POINT3D center;// center of circle
	POINT3D anchor;// where circle is anchored
	POINT3D *pA, *pB, *nA, *nB;
	int i;
	// IPOINT2D* texRep = pBuildInfo->texRep;
	GLfloat radius = pBuildInfo->radius;
	//BOOL bTexture = pBuildInfo->bTexture;

	slices = pBuildInfo->nSlices;
	stacks = slices / 2;

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	s_delta = s_end - s_start;

	// if(bTexture) {
	// 	// calculate 't' texture coords
	// 	for(i = 0; i <= slices; i++) {
	// 		tex_t[i] = (GLfloat) i * texRep->y / slices;
	// 	}
	// }

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
	CalcNormals(pi, n0, &center, numPoints);

	// // initial 's' texture coordinate
	// tex_s[0] = s_start;

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
		TransformCircle(angle, pi, pB, numPoints + 1, &anchor);

		// calculate normals
		center = pB[numPoints];
		CalcNormals(pB, nB, &center, numPoints);

		// // calculate next 's' texture coord
		// tex_s[1] = (GLfloat) s_start + s_delta * i / stacks;

		// now we've got points and normals, ready to be quadstrip'd
		// MakeQuadStrip(pA, pB, nA, nB, bTexture, tex_s, tex_t, numPoints);
		MakeQuadStrip(pA, pB, nA, nB, numPoints);

		// reset pointers
		pA = pB;
		nA = nB;
		pB = (pB == p0) ? p1 : p0;
		nB = (nB == n0) ? n1 : n0;
		// tex_s[0] = tex_s[1];
	}

	glEndList();
}

void BALLJOINT_OBJECT::Build(OBJECT_BUILD_INFO* pBuildInfo, int notch,
                             float s_start, float s_end) {
	GLfloat ballRadius;
	GLfloat angle, delta_a, startAng, theta;
	GLint numPoints;
	GLfloat s_delta;
	POINT3D pi0[CACHE_SIZE];// 2 circles of untransformed points
	POINT3D pi1[CACHE_SIZE];
	POINT3D p0[CACHE_SIZE];// 2 rows of transformed points
	POINT3D p1[CACHE_SIZE];
	POINT3D n0[CACHE_SIZE];// 2 rows of normals
	POINT3D n1[CACHE_SIZE];
	float r[CACHE_SIZE];// radii of the circles
	// GLfloat tex_t[CACHE_SIZE];// 't' texture coords
	// GLfloat tex_s[2];         // 's' texture coords
	POINT3D center;// center of circle
	POINT3D anchor;// where circle is anchored
	POINT3D *pA, *pB, *nA, *nB;
	int i, k;
	GLint stacks, slices;
	// IPOINT2D* texRep = pBuildInfo->texRep;
	GLfloat radius = pBuildInfo->radius;
	// BOOL bTexture = pBuildInfo->bTexture;

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

	// if(bTexture) {
	// 	// calculate 't' texture coords
	// 	for(i = 0; i <= slices; i++) {
	// 		tex_t[i] = (GLfloat) i * texRep->y / slices;
	// 	}
	// }

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
	CalcNormals(pi0, n0, &center, numPoints);

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
		TransformCircle(angle, pi1, pB, numPoints, &anchor);

		// calculate normals
		CalcNormals(pB, nB, &center, numPoints);

		// calculate next 's' texture coord
		// tex_s[1] = (GLfloat) s_start + s_delta * i / stacks;

		// now we've got points and normals, ready to be quadstrip'd
		// MakeQuadStrip(pA, pB, nA, nB, bTexture, tex_s, tex_t, numPoints);
		MakeQuadStrip(pA, pB, nA, nB, numPoints);

		// reset pointers
		pA = pB;
		nA = nB;
		pB = (pB == p0) ? p1 : p0;
		nB = (nB == n0) ? n1 : n0;
		// tex_s[0] = tex_s[1];
	}

	glEndList();
}

// 'glu' routines

#ifdef _EXTENSIONS_
#define COS cosf
#define SIN sinf
#define SQRT sqrtf
#else
#define COS cos
#define SIN sin
#define SQRT sqrt
#endif

/**************************************************************************\
* BuildCylinder
*
\**************************************************************************/
void PIPE_OBJECT::Build(OBJECT_BUILD_INFO* pBuildInfo, float length, float s_start,
                        float s_end) {
	GLint stacks, slices;
	GLint i, j;
	GLfloat sinCache[CACHE_SIZE];
	GLfloat cosCache[CACHE_SIZE];
	GLfloat sinCache2[CACHE_SIZE];
	GLfloat cosCache2[CACHE_SIZE];
	GLfloat angle;
	GLfloat zLow, zHigh;
	GLfloat zNormal;
	GLfloat s_delta;
	// IPOINT2D* texRep = pBuildInfo->texRep;
	GLfloat radius = pBuildInfo->radius;
	// BOOL bTexture = pBuildInfo->bTexture;

	slices = pBuildInfo->nSlices;
	stacks = (int) ROUND_UP((length / pBuildInfo->divSize) * (float) slices);

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	s_delta = s_end - s_start;

	for(i = 0; i < slices; i++) {
		angle = 2 * PI * i / slices;
		sinCache2[i] = (float) SIN(angle);
		cosCache2[i] = (float) COS(angle);
		sinCache[i] = (float) SIN(angle);
		cosCache[i] = (float) COS(angle);
	}

	sinCache[slices] = sinCache[0];
	cosCache[slices] = cosCache[0];
	sinCache2[slices] = sinCache2[0];
	cosCache2[slices] = cosCache2[0];

	glNewList(listNum, GL_COMPILE);

	for(j = 0; j < stacks; j++) {
		zLow = j * length / stacks;
		zHigh = (j + 1) * length / stacks;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= slices; i++) {
			glNormal3f(sinCache2[i], cosCache2[i], zNormal);
			// if(bTexture) {
			// 	glTexCoord2f((float) s_start + s_delta * j / stacks,
			// 	             (float) i * texRep->y / slices);
			// }
			glVertex3f(radius * sinCache[i],
			           radius * cosCache[i], zLow);
			// if(bTexture) {
			// 	glTexCoord2f((float) s_start + s_delta * (j + 1) / stacks,
			// 	             (float) i * texRep->y / slices);
			// }
			glVertex3f(radius * sinCache[i],
			           radius * cosCache[i], zHigh);
		}
		glEnd();
	}

	glEndList();
}

void SPHERE_OBJECT::Build(OBJECT_BUILD_INFO* pBuildInfo, GLfloat radius,
                          GLfloat s_start, GLfloat s_end) {
	GLint i, j;
	GLfloat sinCache1a[CACHE_SIZE];
	GLfloat cosCache1a[CACHE_SIZE];
	GLfloat sinCache2a[CACHE_SIZE];
	GLfloat cosCache2a[CACHE_SIZE];
	GLfloat sinCache1b[CACHE_SIZE];
	GLfloat cosCache1b[CACHE_SIZE];
	GLfloat sinCache2b[CACHE_SIZE];
	GLfloat cosCache2b[CACHE_SIZE];
	GLfloat angle;
	GLfloat zLow, zHigh;
	GLfloat sintemp1, sintemp2, sintemp3, sintemp4;
	GLfloat costemp3, costemp4;
	GLfloat s_delta;
	GLint start, finish;
	GLint stacks, slices;
	//BOOL bTexture = pBuildInfo->bTexture;
	//	IPOINT2D* texRep = pBuildInfo->texRep;

	slices = pBuildInfo->nSlices;
	stacks = slices;
	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	// invert sense of s - it seems the glu sphere is not built similarly
	// to the glu cylinder
	// (this probably means stacks don't grow along +z - check it out)
	s_delta = s_start;
	s_start = s_end;
	s_end = s_delta;

	s_delta = s_end - s_start;

	/* Cache is the vertex locations cache */
	/* Cache2 is the various normals at the vertices themselves */

	for(i = 0; i < slices; i++) {
		angle = 2 * PI * i / slices;
		sinCache1a[i] = (float) SIN(angle);
		cosCache1a[i] = (float) COS(angle);
		sinCache2a[i] = sinCache1a[i];
		cosCache2a[i] = cosCache1a[i];
	}

	for(j = 0; j <= stacks; j++) {
		angle = PI * j / stacks;
		sinCache2b[j] = (float) SIN(angle);
		cosCache2b[j] = (float) COS(angle);
		sinCache1b[j] = radius * (float) SIN(angle);
		cosCache1b[j] = radius * (float) COS(angle);
	}
	/* Make sure it comes to a point */
	sinCache1b[0] = 0.0f;
	sinCache1b[stacks] = 0.0f;

	sinCache1a[slices] = sinCache1a[0];
	cosCache1a[slices] = cosCache1a[0];
	sinCache2a[slices] = sinCache2a[0];
	cosCache2a[slices] = cosCache2a[0];

	glNewList(listNum, GL_COMPILE);

	/* Do ends of sphere as TRIANGLE_FAN's (if not bTexture)
        ** We don't do it when bTexture because we need to respecify the
        ** texture coordinates of the apex for every adjacent vertex (because
        ** it isn't a constant for that point)
        */
	// if(!bTexture) {
	start = 1;
	finish = stacks - 1;

	/* Low end first (j == 0 iteration) */
	sintemp2 = sinCache1b[1];
	zHigh = cosCache1b[1];
	sintemp3 = sinCache2b[1];
	costemp3 = cosCache2b[1];
	glNormal3f(sinCache2a[0] * sinCache2b[0],
	           cosCache2a[0] * sinCache2b[0],
	           cosCache2b[0]);

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, radius);

	for(i = slices; i >= 0; i--) {
		glNormal3f(sinCache2a[i] * sintemp3,
		           cosCache2a[i] * sintemp3,
		           costemp3);
		glVertex3f(sintemp2 * sinCache1a[i],
		           sintemp2 * cosCache1a[i], zHigh);
	}
	glEnd();

	/* High end next (j == stacks-1 iteration) */
	sintemp2 = sinCache1b[stacks - 1];
	zHigh = cosCache1b[stacks - 1];
	sintemp3 = sinCache2b[stacks - 1];
	costemp3 = cosCache2b[stacks - 1];
	glNormal3f(sinCache2a[stacks] * sinCache2b[stacks],
	           cosCache2a[stacks] * sinCache2b[stacks],
	           cosCache2b[stacks]);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, -radius);
	for(i = 0; i <= slices; i++) {
		glNormal3f(sinCache2a[i] * sintemp3,
		           cosCache2a[i] * sintemp3,
		           costemp3);
		glVertex3f(sintemp2 * sinCache1a[i],
		           sintemp2 * cosCache1a[i], zHigh);
	}
	glEnd();
	// } else {
	// 	start = 0;
	// 	finish = stacks;
	// }
	for(j = start; j < finish; j++) {
		zLow = cosCache1b[j];
		zHigh = cosCache1b[j + 1];
		sintemp1 = sinCache1b[j];
		sintemp2 = sinCache1b[j + 1];
		sintemp3 = sinCache2b[j + 1];
		costemp3 = cosCache2b[j + 1];
		sintemp4 = sinCache2b[j];
		costemp4 = cosCache2b[j];

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= slices; i++) {
			glNormal3f(sinCache2a[i] * sintemp3,
			           cosCache2a[i] * sintemp3,
			           costemp3);
			// if(bTexture) {
			// 	glTexCoord2f((float) s_start + s_delta * (j + 1) / stacks,
			// 	             (float) i * texRep->y / slices);
			// }
			glVertex3f(sintemp2 * sinCache1a[i],
			           sintemp2 * cosCache1a[i], zHigh);
			glNormal3f(sinCache2a[i] * sintemp4,
			           cosCache2a[i] * sintemp4,
			           costemp4);
			// if(bTexture) {
			// 	glTexCoord2f((float) s_start + s_delta * j / stacks,
			// 	             (float) i * texRep->y / slices);
			// }
			glVertex3f(sintemp1 * sinCache1a[i],
			           sintemp1 * cosCache1a[i], zLow);
		}
		glEnd();
	}

	glEndList();
}
