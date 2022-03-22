/**
 * @file structs.h
 * @author Alex "FaceFTW" Westerman
 * @brief Relevant Structs
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
#include <GL/gl.h>

#include "constants.h"

struct _GLConfig;
struct GenConfig;
struct _point2d;
struct _ipoint2d;
struct _point3d;
struct _ipoint3d;

/**
 * @brief GL View Configuration Vars. 
 * Seemed sensible to place them in a struct and accessed as necessary
 */
typedef struct _GLConfig {
	float viewAngle = FOV;           // field of view angle for height
	float zNear = Z_NEAR;            // near z clip value
	float zFar = Z_FAR;              // far z clip value
	float zTrans = Z_TRANS;          // z translation
	float yRot = Y_ROT;              // current yRotation
	float viewDist = -1 * Z_TRANS;   // viewing distance, usually -zTrans
	int numDiv = NUM_DIV;            // # grid divisions in x,y,z
	float divSize = DIV_SIZE;        // distance between divisions
	float aspectRatio = ASPECT_RATIO;// x/y window aspect ratio
	float worldx = 0;                // view area in world space x
	float worldy = 0;                // view area in world space y
	float worldz = 0;                // view area in world space z
	int winWidth = WIN_WIDTH;        //Window Width
	int winHeight = WIN_HEIGHT;      //Window Height
} GLConfig;

typedef struct _GLConfig* GLConfigPtr;


/**
 * @brief Struct with general config vars
 * Planned to be used for cmd args
 */
typedef struct GenConfig {

} Config;

/**
 * @brief Defines a point in 2D space with floating-point precision
 */
typedef struct _point2d {
	GLfloat x;//The x position
	GLfloat y;//The y position
} POINT2D;

/**
 * @brief Defines a point in 2D space with integer precision
 */
typedef struct _ipoint2d {
	int x;//The x position
	int y;//The y position
} IPOINT2D;

/**
 * @brief Defines a point in 3D space with floating-point precision
 */
typedef struct _point3d {
	GLfloat x;//The x position
	GLfloat y;//The y position
	GLfloat z;//The z position
} POINT3D;

/**
 * @brief Defines a point in 3D space with integer precision
 */
typedef struct _ipoint3d {
	int x;//The x position
	int y;//The y position
	int z;//The z position
} IPOINT3D;

/**
 * @brief Defines a Float Matrix Struct
 */
typedef struct _MATRIX {
	GLfloat M[4][4];
} MATRIX;

typedef struct strRGBA {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} RGBA;

/**
 * @brief 
 * 
 */
typedef struct _MATERIAL {
	RGBA ka;
	RGBA kd;
	RGBA ks;
	GLfloat specExp;
} MATERIAL;