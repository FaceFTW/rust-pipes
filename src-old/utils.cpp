
#include "include/utils.h"
#include <pcg_basic.h>

using namespace std;

int iRand(int max) { return pcg32_boundedrand(max); }

//Assume max > min
int iRand2(int min, int max) {
	int diff = max - min;
	return min + pcg32_boundedrand(diff);
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

Direction invertDirection(Direction dir) {
	switch(dir) {
		case DIR_X_PLUS: return DIR_X_MINUS;
		case DIR_X_MINUS: return DIR_X_PLUS;
		case DIR_Y_PLUS: return DIR_Y_MINUS;
		case DIR_Y_MINUS: return DIR_Y_PLUS;
		case DIR_Z_PLUS: return DIR_Z_MINUS;
		case DIR_Z_MINUS: return DIR_Z_PLUS;
		default: return DIR_NONE;
	}
}