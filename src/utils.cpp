/**
 * @file utils.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
// #include <GL/gl.h>
#include <math.h>

#include "include/utils.h"

int iRand(int max) { return rand() % (max + 1); }

int iRand2(int min, int max) {
	if(min == max) return min;
	else if(max < min) {
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (rand() % (max - min + 1));
}

float fRand(float min, float max) {
	float diff;

	diff = max - min;
	return min + (diff * (((float) rand()) / ((float) (RAND_MAX))));
}

Axis getAxisFromDirection(Direction dir) {
	switch(dir) {
		case DIR_X_PLUS:
		case DIR_X_MINUS: return AXIS_X;
		case DIR_Y_PLUS:
		case DIR_Y_MINUS: return AXIS_Y;
		case DIR_Z_PLUS:
		case DIR_Z_MINUS: return AXIS_Z;
		default: return AXIS_X;//should never happen
	}
}