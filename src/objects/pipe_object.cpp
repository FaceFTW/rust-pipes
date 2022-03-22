/**
 * @file pipe_object.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */
#include <math.h>

#include "../include/objects/pipe_object.h"
#include "../include/utils.h"

PipeObject::PipeObject(ObjectBuildInfo* state, float length) : Object() {
	build(state, length, 0.0f, 0.0f);
}

PipeObject::PipeObject(ObjectBuildInfo* state, float length, float start_s, float end_s) : Object() {
	build(state, length, start_s, end_s);
}

void PipeObject::build(ObjectBuildInfo* pBuildInfo, float length, float s_start,
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
	GLfloat radius = pBuildInfo->radius;

	slices = pBuildInfo->nSlices;
	stacks = (int) ROUND_UP((length / pBuildInfo->divSize) * (float) slices);

	if(slices >= CACHE_SIZE) slices = CACHE_SIZE - 1;
	if(stacks >= CACHE_SIZE) stacks = CACHE_SIZE - 1;

	zNormal = 0.0f;

	s_delta = s_end - s_start;

	for(i = 0; i < slices; i++) {
		angle = 2 * PI * i / slices;
		sinCache2[i] = (float) sin(angle);
		cosCache2[i] = (float) cos(angle);
		sinCache[i] = (float) sin(angle);
		cosCache[i] = (float) cos(angle);
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
			glVertex3f(radius * sinCache[i],
			           radius * cosCache[i], zLow);
			glVertex3f(radius * sinCache[i],
			           radius * cosCache[i], zHigh);
		}
		glEnd();
	}

	glEndList();
}
