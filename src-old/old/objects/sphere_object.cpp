/**
 * @file sphere_object.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "../include/objects/sphere_object.h"
#include "../include/constants.h"
#include <math.h>

SphereObject::SphereObject(ObjectBuildInfo* state, float radius, float start_s, float end_s) : Object() {
	build(state, radius, start_s, end_s);
}

SphereObject::SphereObject(ObjectBuildInfo* state, float radius) : Object() {
	build(state, radius, 0.0f, 0.0f);
};

void SphereObject::build(ObjectBuildInfo* pBuildInfo, GLfloat radius,
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
		sinCache1a[i] = (float) sin(angle);
		cosCache1a[i] = (float) cos(angle);
		sinCache2a[i] = sinCache1a[i];
		cosCache2a[i] = cosCache1a[i];
	}

	for(j = 0; j <= stacks; j++) {
		angle = PI * j / stacks;
		sinCache2b[j] = (float) sin(angle);
		cosCache2b[j] = (float) cos(angle);
		sinCache1b[j] = radius * (float) sin(angle);
		cosCache1b[j] = radius * (float) cos(angle);
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
			glVertex3f(sintemp2 * sinCache1a[i],
			           sintemp2 * cosCache1a[i], zHigh);
			glNormal3f(sinCache2a[i] * sintemp4,
			           cosCache2a[i] * sintemp4,
			           costemp4);
			glVertex3f(sintemp1 * sinCache1a[i],
			           sintemp1 * cosCache1a[i], zLow);
		}
		glEnd();
	}

	glEndList();
}
