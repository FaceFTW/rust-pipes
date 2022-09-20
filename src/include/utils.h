#pragma once
#include "glad/glad.h"
// #include <cmath>
#include <iostream>

#ifndef __utils_h_
#define __utils_h_

/******TYPEDEFS********/
typedef unsigned int uint;

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

#define TEAPOT_CHANCE 0.1//Will change eventually, just for testing
#define TURN_CHANCE 0.1  //Will change eventually, just for testing

#define TEAPOT 66

// If random search takes longer than twice the total number
// of nodes, give up the random search.  There may not be any
// empty nodes.
#define INFINITE_LOOP (2 * NUM_NODE * NUM_NODE * NUM_NODE)
#define MIN_VAL 1
#define MAX_VAL 0

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

/*****OLD ENUMS******/
// // shchemes for choosing directions
// enum {
// 	NORMAL_SCHEME_CHOOSE_DIR_RANDOM,
// 	NORMAL_SCHEME_CHOOSE_DIR_TURN,
// 	NORMAL_SCHEME_CHOOSE_DIR_STRAIGHT
// };

// #define NUM_TEA_MATERIALS 24
// enum {
// 	EMERALD = 0,
// 	JADE,
// 	OBSIDIAN,
// 	PEARL,
// 	RUBY,
// 	TURQUOISE,
// 	BRASS,
// 	BRONZE,
// 	CHROME,
// 	COPPER,
// 	GOLD,
// 	SILVER,
// 	BLACK_PLASTIC,
// 	CYAN_PLASTIC,
// 	GREEN_PLASTIC,
// 	RED_PLASTIC,
// 	WHITE_PLASTIC,
// 	YELLOW_PLASTIC,
// 	BLACK_RUBBER,
// 	CYAN_RUBBER,
// 	GREEN_RUBBER,
// 	RED_RUBBER,
// 	WHITE_RUBBER,
// 	YELLOW_RUBBER
// };

// // 'white' materials, for texturing

// #define NUM_TEX_MATERIALS 4

// enum { BRIGHT_WHITE = NUM_TEA_MATERIALS, WHITE, WARM_WHITE, COOL_WHITE };

#pragma endregion

/******STRUCTS*******/
#pragma region "Struct Typedefs"

typedef struct s_point_t {
	uint x;
	uint y;
	uint z;

	s_point_t() {
		x = 0;
		y = 0;
		z = 0;
	}

	s_point_t(uint _x, uint _y, uint _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	s_point_t(s_point_t* _point) {
		x = _point->x;
		y = _point->y;
		z = _point->z;
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

// Not using interleaved arrays because *why*
typedef struct s_gl_object_data_t {
	GLdouble* vArray;
	GLint vArraySize;

	GLdouble* nArray;
	GLint nArraySize;

	Point* pos;

} GLObjectData;

/******METHOD DEFS******/
#pragma region "Method Declarations"
/**
 * @brief Generates integer random number
 *
 * @param max Maximum number
 * @return int  Random number between 0 and (max-1)
 */
int iRand(int max);

/**
 * @brief Generates integer random number
 *
 * @param min Minimum of Random
 * @param max Maximum of Random
 * @return int Random integer between min and max
 */
int iRand2(int min, int max);

Axis getAxisFromDirection(Direction dir);

#pragma endregion
#endif//__utils_h_