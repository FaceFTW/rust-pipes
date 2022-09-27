#pragma once
#include "glad/glad.h"
#include <cstdint>
#include <iostream>

#ifndef __utils_h_
#define __utils_h_

/******CONFIG STRUCT*******/
//HACK Not sure how safe this is in practice
//TODO investigate because I may be doing an oopsie
//TODO Consider shrinking types to reduce struct size
typedef struct s_appConfig {
	int totalPipes;    //Total number of pipes to build
	int minIter;       //Minimum number of iterations when generating a pipe
	int maxIter;       //Maximum number of iterations when generating a pipe
	int nodeStructSize;//Assume uniform dimensions for now
} AppConfig;

typedef struct s_drawConfig {
	double pipeLength;//Float length of the pipe
	int pipeSlices;//Number of length slices used for pipe nodes (larger -> more circular precision)
	int pipeRadius;
	int sphereSlices;
	int sphereStacks;
	int sphereRadius;

} DrawConfig;

/******CONSTANTS*******/
#pragma region "Constants"

// #if __has_include(<numbers>)
// #include <numbers>
// #define PI std::numbers::pi
// #define ONE_OVER_PI std::numbers::inv_pi
// #define ROOT_TWO std::numbers::sqrt2
// #else
#define PI 3.14159265358979323846
#define ONE_OVER_PI 1 / PI
#define ROOT_TWO 1.41421356237309504880
// #endif

#define TEAPOT 66

#pragma endregion

/******ENUMS*******/
#pragma region "Enum Typedefs"

typedef enum e_axis { AXIS_X, AXIS_Y, AXIS_Z } Axis;

typedef enum e_direction {
	DIR_X_PLUS = 0,
	DIR_X_MINUS = 1,
	DIR_Y_PLUS = 2,
	DIR_Y_MINUS = 3,
	DIR_Z_PLUS = 4,
	DIR_Z_MINUS = 5,
	DIR_NONE
} Direction;

#pragma endregion

/******STRUCTS*******/
#pragma region "Struct Typedefs"

typedef struct s_point_t {
	unsigned int x;
	unsigned int y;
	unsigned int z;

	s_point_t() {
		x = 0;
		y = 0;
		z = 0;
	}

	s_point_t(unsigned int _x, unsigned int _y, unsigned int _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	//Neighbor Constructor
	s_point_t(s_point_t* _point, Direction dir) {
		switch(dir) {
			case DIR_X_PLUS:
				x = (_point->x) + 1;
				y = (_point->y);
				z = (_point->z);
				break;
			case DIR_X_MINUS:
				x = (_point->x) - 1;
				y = (_point->y);
				z = (_point->z);
				break;
			case DIR_Y_PLUS:
				x = (_point->x);
				y = (_point->y) + 1;
				z = (_point->z);
				break;
			case DIR_Y_MINUS:
				x = (_point->x);
				y = (_point->y) - 1;
				z = (_point->z);
				break;
			case DIR_Z_PLUS:
				x = (_point->x);
				y = (_point->y);
				z = (_point->z) + 1;
				break;
			case DIR_Z_MINUS:
				x = (_point->x);
				y = (_point->y);
				z = (_point->z) - 1;
				break;
			default:
				x = (_point->x);
				y = (_point->y);
				z = (_point->z);
				break;
		}
	}
	friend std::ostream& operator<<(std::ostream& os, s_point_t point) {
		os << '[' << point.x << ", " << point.y << ", " << point.z << ", " << ']';
		return os;
	}

} Point;

/******METHOD DEFS******/
#pragma region "Method Declarations"

int iRand(int max);
int iRand2(int min, int max);
Axis getAxisFromDirection(Direction dir);
Direction invertDirection(Direction dir);

#pragma endregion
#endif//__utils_h_