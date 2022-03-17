/**
 * @file pipe_flexy.h
 * @author Alex "FaceFTW" Westerman
 * @brief Definition of Flexy Pipes (whatever that means??)
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

//NOTE this is mostly a copy of FPIPE for now, will likely change over time

#ifndef __pipe_flexy_h__
#define __pipe_flexy_h__

//TODO Find if refs exist
//#include "sscommon.h"

#include "../pipe_main.h"
#include "pipe.h"

#include "../state/state.h"
#include "../glstuff.h"

// continuity types
enum {
	CONT_1 = 0,
	CONT_2
};

// cap types
enum {
	START_CAP = 0,
	END_CAP
};

#define CIRCUMFERENCE( r ) \
    ( 2.0f * PI * ((GLfloat) r) )

// drawing schemes
enum {
	SC_EXTRUDED_XC,
	SC_TURNOMANIA,
	SC_VARIABLE_XC,
	SC_COUNT
};

#define TURNOMANIA_PIPE_COUNT 10

/**************************************************************************\
*
* FLEX_PIPE class
*
* - Pipe has position and direction in node array
*
\**************************************************************************/

class FLEX_PIPE : public PIPE {
	public:
	void        SetTexParams(TEXTURE* pTex, IPOINT2D* pTexRep);

	protected:
	XC* xcCur, * xcEnd;  // current xc, end xc
	EVAL* pEval;
	GLfloat     zTrans;         // pos'n of pipe back along current dir,
								// from current node
	FLEX_PIPE(STATE* state);
	~FLEX_PIPE();
	void        ChooseXCProfile();
	void        DrawExtrudedXCObject(float length);
	void        DrawXCElbow(int newDir, float radius);
	void        DrawCap(int type);

	private:
	int         nSlices;        // intended # of slices around an xc (based
								// on tessLevel)
	int         tangent;        // current tangent at curXC (usually PLUS_Z)
	GLfloat     s_start, s_end;
	GLfloat     t_start, t_end;
	GLfloat     s_length;       // length in WC's of texture width
	GLfloat     evalDivSize;    // ~size in WC's of an eval division
	void        CalcEvalLengthParams(float length);
};

class REGULAR_FLEX_PIPE : public FLEX_PIPE {
	public:
	float       turnFactorMin;  // describes degree of bend in an elbow
	float       turnFactorMax;  //  [0-1]

	REGULAR_FLEX_PIPE(STATE* state);
	void        Start();
	void        Draw();
};

class TURNING_FLEX_PIPE : public FLEX_PIPE {
	public:
	TURNING_FLEX_PIPE(STATE* state);
	void        Start();
	void        Draw();
};

#endif // __pipe_flexy_h__