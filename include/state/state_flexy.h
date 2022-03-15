/**
 * @file state_flexy.h
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

//NOTE Mostly a copy of FSTATE.H for now, will get changed over time

#ifndef __state_flexy_h__
#define __state_flexy_h__


//TODO Find any dependencies
//#include "sscommon.h"
#include "state.h"
#include "../pipe/pipe.h"

class PIPE;
class STATE;

class FLEX_STATE {
public:
    int             scheme;         // current drawing scheme (right now this
                                    // is a per-frame thing)
    BOOL            bTexture;       // mf: repetition
    FLEX_STATE( STATE *pState );
    PIPE*           NewPipe( STATE *pState );
    void            Reset();
    BOOL            OKToUseChase();
    int             GetMaxPipesPerFrame();
};

#endif // __state_flexy_h__
