/**
 * @file glstuff.h
 * @author Alex "FaceFTW" Westerman
 * @brief Contains all of the relevant GL Bindings
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

//NOTE Most of this code originates from EVAL.H and XC.H
//These will likely be updated for more recent versions of OpenGL

#ifndef __glstuff_h_
#define __glstuff_h_

//*********************************
// XC.H Stuff
//*********************************

// useful for xc-coords
enum {
    RIGHT = 0,
    TOP,
    LEFT,
    BOTTOM
};

// Cross_section (xc) class

class XC {
public:
    GLfloat     xLeft, xRight;  // bounding box
    GLfloat     yTop, yBottom;
    int         numPts;
    POINT2D     *pts;        // CW points around the xc, from +x

    XC( int numPts );
    XC( const XC& xc );
    XC( XC *xc );
    ~XC();
    void        Scale( float scale );
    float       MaxExtent();
    float       MinTurnRadius( int relDir );
    void        CalcArcACValues90( int dir, float r, float *acPts );
    void        CalcArcACValuesByDistance(  float *acPts );
    void        ConvertPtsZ( POINT3D *pts, float z );
protected:
    void        CalcBoundingBox();
};

// Specific xc's derived from base xc class

class ELLIPTICAL_XC : public XC {
public:
    ELLIPTICAL_XC( float r1, float r2 );
    ~ELLIPTICAL_XC();
private:
    void SetControlPoints( float r1, float r2 );
};

class RANDOM4ARC_XC : public XC {
public:
    RANDOM4ARC_XC( float r );
    ~RANDOM4ARC_XC();
private:
    void SetControlPoints( float radius );
};

//*********************************
// EVAL.H Stuff
//*********************************

#define MAX_UORDER      5 // this is per section
#define MAX_VORDER      5
#define MAX_USECTIONS   4
#define MAX_XC_PTS      (MAX_UORDER * MAX_USECTIONS)

#define TEX_ORDER       2
#define EVAL_ARC_ORDER  4
#define EVAL_CYLINDER_ORDER 2
#define EVAL_ELBOW_ORDER    4

// # of components (eg. arcs) to form a complete cross-section
#define EVAL_XC_CIRC_SECTION_COUNT 4

#define EVAL_XC_POINT_COUNT ( (EVAL_ARC_ORDER-1)*4 + 1 )

#define EVAL_CIRC_ARC_CONTROL 0.56f // for r=1

/**************************************************************************\
*
* EVAL class
*
* - Evaluator composed of one or more sections that are evaluated
*   separately with OpenGL evaluators
*
\**************************************************************************/

class EVAL {
public:
    EVAL( BOOL bTexture );
    ~EVAL();
    int         numSections;    // number of cross-sectional sections
    int         uOrder, vOrder;
        // assumed: all sections same order - uOrder is per
        // section; sections share vertex and texture control points
    int         uDiv, vDiv;    // figured out one level up ?
    POINT3D     *pts;          // vertex control points
    // - texture always order 2 for s and t (linear mapping)
    BOOL        bTexture;
    TEX_POINT2D *texPts;       // texture control points

    void        Evaluate(); // evaluate/render the object
    void        SetVertexCtrlPtsXCTranslate( POINT3D *pts, float length, 
                                             XC *xcStart, XC *xcEnd );
    void        SetTextureControlPoints( float s_start, float s_end,
                                         float t_start, float t_end );
    void        ProcessXCPrimLinear( XC *xcStart, XC *xcEnd, float length );
    void        ProcessXCPrimBendSimple( XC *xcCur, int dir, float radius );
    void        ProcessXCPrimSingularity( XC *xcCur, float length, 
                                          BOOL bOpening );
};

extern void ResetEvaluator( BOOL bTexture );

//*********************************
// Stuff from VIEW.H
//*********************************
typedef struct {
    float viewAngle;            // field of view angle for height
    float zNear;                // near z clip value
    float zFar;                 // far z clip value
} Perspective;  // perspective view description

class VIEW {
public:
    float       zTrans;         // z translation
    float       yRot;           // current yRotation
    float       viewDist;       // viewing distance, usually -zTrans
    int         numDiv;         // # grid divisions in x,y,z
    float       divSize;        // distance between divisions
    ISIZE       winSize;        // window size in pixels

    VIEW();
    BOOL        SetWinSize( int width, int height );
    void        SetGLView();
    void        CalcNodeArraySize( IPOINT3D *pNodeDim );
    void        SetProjMatrix();
    void        IncrementSceneRotation();
private:
    BOOL        bProjMode;      // projection mode
    Perspective persp;          // perspective view description
    float       aspectRatio;    // x/y window aspect ratio
    POINT3D     world;          // view area in world space
};


#endif //__glstuff_h_