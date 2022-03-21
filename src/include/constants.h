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

#define ROOT_TWO 1.414213562373f

#define CACHE_SIZE 100

#define PI 3.14159265358979323846f
// double version of PI
#define PI_D 3.14159265358979323846264338327950288419716939937510
#define ONE_OVER_PI (1.0f / PI)

#define ZERO_EPS 0.00000001

// pipe drawing status
enum {
	PIPE_ACTIVE,
	PIPE_STUCK,
	PIPE_OUT_OF_NODES
};

// pipe types
enum {
	TYPE_NORMAL,
	TYPE_FLEX_REGULAR,
	TYPE_FLEX_TURNING
};

// ways pipe choose directions
enum {
	CHOOSE_DIR_RANDOM_WEIGHTED,
	CHOOSE_DIR_CHASE// when chasing a lead pipe
};

// ways pipe choose start positions
enum {
	CHOOSE_STARTPOS_RANDOM,
	CHOOSE_STARTPOS_FURTHEST// furthest from last position
};

/******** materials ********************************/

// 24 tea materials (from teapots app)

#define NUM_TEA_MATERIALS 24

enum {
	EMERALD = 0,
	JADE,
	OBSIDIAN,
	PEARL,
	RUBY,
	TURQUOISE,
	BRASS,
	BRONZE,
	CHROME,
	COPPER,
	GOLD,
	SILVER,
	BLACK_PLASTIC,
	CYAN_PLASTIC,
	GREEN_PLASTIC,
	RED_PLASTIC,
	WHITE_PLASTIC,
	YELLOW_PLASTIC,
	BLACK_RUBBER,
	CYAN_RUBBER,
	GREEN_RUBBER,
	RED_RUBBER,
	WHITE_RUBBER,
	YELLOW_RUBBER
};

// 'white' materials, for texturing

#define NUM_TEX_MATERIALS 4

enum {
	BRIGHT_WHITE = NUM_TEA_MATERIALS,
	WHITE,
	WARM_WHITE,
	COOL_WHITE
};

#endif//__constants_h_