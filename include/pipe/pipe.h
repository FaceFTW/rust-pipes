/**
 * @file pipe.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

//NOTE This is currently a copy of PIPE.h, will likely change maybe idk

#ifndef __pipe_h_
#define __pipe_h_

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
    CHOOSE_DIR_CHASE // when chasing a lead pipe
};

// ways pipe choose start positions
enum {
    CHOOSE_STARTPOS_RANDOM,
    CHOOSE_STARTPOS_FURTHEST // furthest from last position
};
/**************************************************************************\
*
* PIPE class
*
* - Describes a pipe that draws thru the node array
* - Could have more than one pipe drawing in each array at same time
* - Pipe has position and direction in node array
*
\**************************************************************************/

class STATE;

class PIPE {
public:
    int         type;
    IPOINT3D    curPos;         // current node position of pipe

    STATE       *pState;        // for state value access

    void        SetChooseDirectionMethod( int method );
    void        SetChooseStartPosMethod( int method );
    int         ChooseNewDirection();
    BOOL        IsStuck();      // if pipe is stuck or not
    BOOL        NowhereToRun(){ return status == PIPE_OUT_OF_NODES; }
protected:
    BOOL        bTexture;
    float       radius;         // ideal radius (fluctuates for FPIPE)
    int         status;         // ACTIVE/STUCK/STOPPED, etc.
    int         lastDir;        // last direction taken by pipe
    int         notchVec;       // current notch vector
    PIPE( STATE *state );
    int         weightStraight; // current weighting of going straight
    BOOL        SetStartPos();  // starting node position
    void        ChooseMaterial();
    void        DrawTeapot();
    void        UpdateCurrentPosition( int dir );
    void        TranslateToCurrentPosition();
private:
    int         chooseDirMethod;
    int         chooseStartPosMethod;
    int         GetBestDirsForChase( int *bestDirs );
};

extern void align_plusz( int newDir );
extern GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS];
#endif // __pipe_h_