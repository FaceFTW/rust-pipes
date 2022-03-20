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
#include <math.h>

#include "include/utils.h"

int iRand(int max) {
	return (int) (max * (((float) rand()) / ((float) (RAND_MAX + 1))));
}

int iRand2(int min, int max) {
	if(min == max)
		return min;
	else if(max < min) {
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (int) ((max - min + 1) * (((float) rand()) / ((float) (RAND_MAX + 1))));
}

float fRand(float min, float max) {
	float diff;

	diff = max - min;
	return min + (diff * (((float) rand()) / ((float) (RAND_MAX))));
}