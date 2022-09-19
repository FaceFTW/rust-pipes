
#include <math.h>

#include "include/utils.h"

int iRand(int max) { return rand() % (max); }

int iRand2(int min, int max) {
	if(min == max) return min;
	else if(max < min) {
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (rand() % (max - min));
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