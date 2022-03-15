/**
 * @file pipe_normal.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definition of Normal Pipes
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *  
 */

//NOTE this is mostly a copy of NPIPE.h for now, will likely change over time

#ifndef __pipe_normal_h__
#define __pipe_normal_h__

//TODO find if dependencies exist
//#include "sscommon.h"
#include "../state/state.h"
#include "pipe.h"

class NORMAL_STATE;

class NORMAL_PIPE: public PIPE {
private:
public:
    NORMAL_STATE      *pNState;

    NORMAL_PIPE( STATE *state );
    void        Start();
    GLint       ChooseElbow( int oldDir, int newDir);
    void        DrawJoint( int newDir );
    void        Draw( ); //mf: could take param to draw n sections
    void        DrawStartCap( int newDir );
    void        DrawEndCap();
};

#endif // __pipe_normal_h__
