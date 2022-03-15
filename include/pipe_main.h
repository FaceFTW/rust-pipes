/**
 * @file pipe_main.h
 * @author Alex "FaceFTW Westerman"
 * @brief Entry Point / Main GL Code Header Stub
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

//For now, this is mainly taken from SSPIPES.h, 
//need to clean out and convert calls away from COM and windows stuff
//GL stuff should be preserved


#ifndef __pipe_main_h__
#define __pipe_main_h__

#include <GL/gl.h>
// TODO Find compatibility layer that probably exists
//#include <GL/glu.h>

// TODO What library is this header in?
//#include <GL/glaux.h>

//TODO Find stuff that is dependent on this
//#include <commctrl.h>

//TODO Find stuff dependent on this (probably nothing)
//#include "sscommon.h"

#define DO_TIMING 1
#ifdef DO_TIMING
enum {
    TIMER_START = 0,
    TIMER_STOP,
    TIMER_TIMING,
    TIMER_RESET
};
extern void Timer( int mode );
extern int pipeCount;
#endif


#define iXX -1
#define fXX -0.01f

// These are absolute directions, with origin in center of screen,
// looking down -z

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

#define NUM_DIV 16              // divisions in window in longest dimension

#define MAX_TEXTURES 8




#endif // __pipe_main_h_
