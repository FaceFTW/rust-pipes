/**
 * @file utils.h
 * @author Alex "FaceFTW" Westerman
 * @brief Utility Functions/Defines/Macros
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#pragma once
#ifndef __utils_h_
#define __utils_h_

// #include <GL/gl.h>

/******TYPEDEFS********/
#pragma region "General Typedefs"
typedef unsigned int uint;
#pragma endregionF

/******CONSTANTS*******/
#pragma region "Constants"

const int NUM_DIRS = 6;
#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define TEAPOT_SIZE 2.0
#define FOV 90
#define ASPECT_RATIO 1.0
#define Z_NEAR 1
#define Z_FAR 1000
#define Y_ROT 0.0f
#define iXX -1
#define fXX -0.01f
#define NUM_DIV 16// divisions in window in longest dimension
#define DIV_SIZE 7.0f
#define MAX_TEXTURES 8
#define Z_TRANS -75.0f
#define VIEW_DIST (-1 * Z_TRANS)

#define TEAPOT_CHANCE 0.1//Will change eventually, just for testing
#define TURN_CHANCE 0.1  //Will change eventually, just for testing

#define MAX_DRAW_THREADS 4

#define TEAPOT 66

#define MAX_TESS 3

#define NORMAL_PIPE_COUNT 5
#define NORMAL_TEX_PIPE_COUNT 3

#define NUM_JOINT_STYLES 3

#define NUM_NODE (NUM_DIV - 1)// num nodes in longest dimension

// maximum weighting of going straight for direction choosing functions
#define MAX_WEIGHT_STRAIGHT 100

// If random search takes longer than twice the total number
// of nodes, give up the random search.  There may not be any
// empty nodes.
#define INFINITE_LOOP (2 * NUM_NODE * NUM_NODE * NUM_NODE)
#define MIN_VAL 1
#define MAX_VAL 0

#define ROOT_TWO 1.414213562373f

#define CACHE_SIZE 100

#define PI 3.14159265358979323846f
// double version of PI
#define PI_D 3.14159265358979323846264338327950288419716939937510
#define ONE_OVER_PI (1.0f / PI)

#define ZERO_EPS 0.00000001

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
// enum { JOINT_ELBOW = 0, JOINT_BALL, JOINT_MIXED, JOINT_CYCLE, NUM_JOINTTYPES };

// // styles for pipe joints
// enum { ELBOWS = 0, BALLS, EITHER };

// // joint types
// enum { ELBOW_JOINT = 0, BALL_JOINT };

// // shchemes for choosing directions
// enum {
// 	NORMAL_SCHEME_CHOOSE_DIR_RANDOM,
// 	NORMAL_SCHEME_CHOOSE_DIR_TURN,
// 	NORMAL_SCHEME_CHOOSE_DIR_STRAIGHT
// };

// // pipe drawing status
// enum { PIPE_ACTIVE, PIPE_STUCK, PIPE_OUT_OF_NODES };

// // pipe types
// enum { TYPE_NORMAL, TYPE_FLEX_REGULAR, TYPE_FLEX_TURNING };

// // ways pipe choose directions
// enum {
// 	CHOOSE_DIR_RANDOM_WEIGHTED,
// 	CHOOSE_DIR_CHASE// when chasing a lead pipe
// };

// // ways pipe choose start positions
// enum {
// 	CHOOSE_STARTPOS_RANDOM,
// 	CHOOSE_STARTPOS_FURTHEST// furthest from last position
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
		}
	}

} Point;

/******OLD STRUCTS******/
// /**
//  * @brief GL View Configuration Vars.
//  * Seemed sensible to place them in a struct and accessed as necessary
//  */
// typedef struct _GLConfig {
// 	float viewAngle = FOV;           // field of view angle for height
// 	float zNear = Z_NEAR;            // near z clip value
// 	float zFar = Z_FAR;              // far z clip value
// 	float zTrans = Z_TRANS;          // z translation
// 	float yRot = Y_ROT;              // current yRotation
// 	float viewDist = -1 * Z_TRANS;   // viewing distance, usually -zTrans
// 	int numDiv = NUM_DIV;            // # grid divisions in x,y,z
// 	float divSize = DIV_SIZE;        // distance between divisions
// 	float aspectRatio = ASPECT_RATIO;// x/y window aspect ratio
// 	float worldx = 0;                // view area in world space x
// 	float worldy = 0;                // view area in world space y
// 	float worldz = 0;                // view area in world space z
// 	int winWidth = 0;                //Window Width
// 	int winHeight = 0;               //Window Height
// } GLConfig;

// typedef struct _GLConfig* GLConfigPtr;

// /**
//  * @brief Struct with general config vars
//  * Planned to be used for cmd args
//  */
// typedef struct GenConfig {

// } Config;

// /**
//  * @brief Defines a point in 2D space with floating-point precision
//  */
// typedef struct _point2d {
// 	GLfloat x;//The x position
// 	GLfloat y;//The y position
// } POINT2D;

// /**
//  * @brief Defines a point in 2D space with integer precision
//  */
// typedef struct _ipoint2d {
// 	int x;//The x position
// 	int y;//The y position
// } IPOINT2D;

// /**
//  * @brief Defines a point in 3D space with floating-point precision
//  */
// typedef struct _point3d {
// 	GLfloat x;//The x position
// 	GLfloat y;//The y position
// 	GLfloat z;//The z position
// } POINT3D;

// /**
//  * @brief Defines a point in 3D space with integer precision
//  */
// typedef struct _ipoint3d {
// 	int x;//The x position
// 	int y;//The y position
// 	int z;//The z position
// } IPOINT3D;

// /**
//  * @brief Defines a Float Matrix Struct
//  */
// typedef struct _MATRIX {
// 	GLfloat M[4][4];
// } MATRIX;

// typedef struct strRGBA {
// 	GLfloat r;
// 	GLfloat g;
// 	GLfloat b;
// 	GLfloat a;
// } RGBA;

// /**
//  * @brief
//  *
//  */
// typedef struct _MATERIAL {
// 	RGBA ka;
// 	RGBA kd;
// 	RGBA ks;
// 	GLfloat specExp;
// } MATERIAL;

#pragma endregion

/******MACRO DEFS******/
#pragma region "Macro Definitions"

/**
 * @brief Returns the maximum of two values
 */
#define MAX(a, b) (a > b ? a : b)

/**
 * @brief Returns the minimum of two values
 */
#define MIN(a, b) (a < b ? a : b)

// macro to round up floating values
#define ROUND_UP(fval)                                                                             \
	((((fval) - (float) (int) (fval)) > 0.0f) ? (int) ((fval) + 1.0f) : (int) (fval))

// macros to clamp a value within a range
#define CLAMP_TO_RANGE(a, lo, hi) ((a < lo) ? lo : ((a > hi) ? hi : a))
#define CLAMP_TO_RANGE2(a, lo, hi) (a = (a < lo) ? lo : ((a > hi) ? hi : a))

// degree<->radian macros
#define ONE_OVER_180 (1.0f / 180.0f)
#define SS_DEG_TO_RAD(a) ((a * PI) * ONE_OVER_180)
#define SS_RAD_TO_DEG(a) ((a * 180.0f) * ONE_OVER_PI)

#pragma endregion

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

/**
 * @brief Generates float random number
 *
 * @param min Minimum of Random
 * @param max Maximum of Random
 * @return float Random float between min and max
 */
float fRand(float min, float max);

/*************************************
Sphere Maths
**************************************/

// /**
//  * @brief rotate circle around x-axis, with edge attached to anchor
//  * @param angle
//  * @param inPoint
//  * @param outPoint
//  * @param num
//  * @param anchor
//  */
// void transformCircle(float angle, POINT3D* inPoint, POINT3D* outPoint, GLint num, POINT3D* anchor);

// /**
//  * @brief
//  *
//  * @param p
//  * @param n
//  * @param center
//  * @param num
//  */
// void calcNormals(POINT3D* p, POINT3D* n, POINT3D* center, int num);

// extern POINT3D ss_ptZero;
// void xformPoint(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
// void xformNorm(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
// void matrixIdent(MATRIX*);
// void matrixRotate(MATRIX* m, double xTheta, double yTheta, double zTheta);
// void matrixTranslate(MATRIX*, double xTrans, double yTrans, double zTrans);
// void matrixMult(MATRIX* m1, MATRIX* m2, MATRIX* m3);
// void calcNorm(POINT3D* norm, POINT3D* p1, POINT3D* p2, POINT3D* p3);
// void normalizeNorm(POINT3D*);
// void normalizeNorms(POINT3D*, unsigned long);

// void align_notch(int newDir, int notch);
// // static GLint defCylNotch[NUM_DIRS];
// // static GLfloat alignNotchRot[NUM_DIRS][NUM_DIRS];
// void align_plusy(int oldDir, int newDir);
// // static float RotZ[NUM_DIRS][NUM_DIRS];

// /**
//  * @brief Aligns the z axis along specified direction
//  *  Used for all types of pipes
//  * @param newDir
//  */
// void align_plusz(int newDir);

/**
 * @brief This array tells you which way the notch will be once you make a turn
 * Format: notchTurn[oldDir][newDir][notchVec]
 *
 */
// static GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS];

// #ifdef __cplusplus
// }
// #endif
#pragma endregion
#endif//__utils_h_