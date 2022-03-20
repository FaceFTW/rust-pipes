/**
 * @file constants.h
 * @author Alex "FaceFTW" Westerman
 * @brief Constant Macros/Defines
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __constants_h_
#define __constants_h_

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

//Direction Enum
enum {
	PLUS_X = 0,
	MINUS_X,
	PLUS_Y,
	MINUS_Y,
	PLUS_Z,
	MINUS_Z,
	NUM_DIRS,
	DIR_NONE,
	DIR_STRAIGHT
};

enum {
	JOINT_ELBOW = 0,
	JOINT_BALL,
	JOINT_MIXED,
	JOINT_CYCLE,
	NUM_JOINTTYPES
};

#define MAX_DRAW_THREADS 4

#define TEAPOT 66

#define MAX_TESS 3

#define NORMAL_PIPE_COUNT 5
#define NORMAL_TEX_PIPE_COUNT 3

#define NUM_JOINT_STYLES 3

// styles for pipe joints
enum {
	ELBOWS = 0,
	BALLS,
	EITHER
};

// joint types
enum {
	ELBOW_JOINT = 0,
	BALL_JOINT
};

// shchemes for choosing directions
enum {
	NORMAL_SCHEME_CHOOSE_DIR_RANDOM,
	NORMAL_SCHEME_CHOOSE_DIR_TURN,
	NORMAL_SCHEME_CHOOSE_DIR_STRAIGHT
};

#define NUM_NODE (NUM_DIV - 1)// num nodes in longest dimension

// maximum weighting of going straight for direction choosing functions
#define MAX_WEIGHT_STRAIGHT 100

// If random search takes longer than twice the total number
// of nodes, give up the random search.  There may not be any
// empty nodes.
#define INFINITE_LOOP (2 * NUM_NODE * NUM_NODE * NUM_NODE)
#define MIN_VAL 1
#define MAX_VAL 0

#endif//__constants_h_