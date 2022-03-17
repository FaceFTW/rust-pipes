/**
 * @file glstuff.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief A bunch of code related to OpenGL implementation
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

//NOTE this is a file that will have a *bunch* of copied code from 
// EVAL.CXX, XC.CXX, and VIEW.CXX. This will get refactored as we go

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//TODO I think this header exists
#include <sys/types.h>
#include <time.h>
//TODO is this header in linux?
#include <sys/timeb.h>
//TODO REMOVE WINDOWS REFS
//#include <windows.h>
#include <GL/gl.h>
// #include <GL/glu.h>
// #include <GL/glaux.h>
#include <float.h>

//TODO find references
// #include "sscommon.h"
#include "include/pipe_main.h"
#include "include/glstuff.h"

//#define EVAL_DBG 1

typedef enum {
	X_PLANE = 0,
	Y_PLANE,
	Z_PLANE
};

#define EVAL_VSIZE 3  // vertex size in floats

#define TMAJOR_ORDER 2
#define TMINOR_ORDER 2

#define VDIM 3
#define TDIM 2

//forwards
#if EVAL_DBG
static void DrawPoints(int num, POINT3D* pts);
#endif
static void RotatePointSet(POINT3D* inPts, int numPts, float angle, int dir,
					  float radius, POINT3D* outPts);
static void ExtrudePointSetDir(POINT3D* inPts, int numPts, float* acPts,
					  int dir, POINT3D* outPts);


/**************************************************************************\
* EVAL
*
* Evaluator constructor
*
\**************************************************************************/

EVAL::EVAL(BOOL bTex) {
	bTexture = bTex;

	// Allocate points buffer

// might want to use less than max in some cases
	int size = MAX_USECTIONS * MAX_UORDER * MAX_VORDER * sizeof(POINT3D);
	pts = (POINT3D*)LocalAlloc(LMEM_FIXED, size);
	SS_ASSERT(pts != NULL, "EVAL constructor\n");

	// Alloc texture points buffer

	if (bTexture) {
		size = MAX_USECTIONS * TEX_ORDER * TEX_ORDER * sizeof(TEX_POINT2D);
		texPts = (TEX_POINT2D*)LocalAlloc(LMEM_FIXED, size);
		SS_ASSERT(texPts != NULL, "EVAL constructor\n");
	}

	ResetEvaluator(bTexture);
}

/**************************************************************************\
* ~EVAL
*
* Evaluator destructor
*
* Frees up memory
*
\**************************************************************************/

EVAL::~EVAL() {
	LocalFree(pts);
	if (bTexture)
		LocalFree(texPts);
}

/**************************************************************************\
* Reset
*
* Reset evaluator to generate 3d vertices and vertex normals
*
\**************************************************************************/

void
ResetEvaluator(BOOL bTexture) {
	if (bTexture) {
		glEnable(GL_MAP2_TEXTURE_COORD_2);
	}
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);
	glFrontFace(GL_CW); // cuz
// if mixing Normal and Flex, have to watch out for this, cuz normal
// needs CCW
}

/**************************************************************************\
* SetTextureControlPoints
*
* Set texture control point net
*
* This sets up 'numSections' sets of texture coordinate control points, based
* on starting and ending s and t values.
*
* s coords run along pipe direction, t coords run around circumference
*
\**************************************************************************/

void
EVAL::SetTextureControlPoints(float s_start, float s_end,
							  float t_start, float t_end) {
	int i;
	TEX_POINT2D* ptexPts = texPts;
	GLfloat t_delta = (t_end - t_start) / numSections;
	GLfloat t = t_start;

	// calc ctrl pts for each quadrant
	for (i = 0; i < numSections; i++, ptexPts += (TDIM * TDIM)) {
		// s, t coords
		ptexPts[0].t = ptexPts[2].t = t;
		t += t_delta;
		ptexPts[1].t = ptexPts[3].t = t;
		ptexPts[0].s = ptexPts[1].s = s_start;
		ptexPts[2].s = ptexPts[3].s = s_end;
	}
}

/**************************************************************************\
* SetVertexCtrlPtsXCTranslate
*
* Builds 3D control eval control net from 2 xcObjs displaced along the
* z-axis by 'length'.
*
* First xc used to generate points in z=0 plane.
* Second xc generates points in z=length plane.
* ! Replicates the last point around each u.
*
\**************************************************************************/

void
EVAL::SetVertexCtrlPtsXCTranslate(POINT3D* pts, float length,
							 XC* xcStart, XC* xcEnd) {
	int i;
	POINT2D* ptsStart, * ptsEnd;
	POINT3D* pts1, * pts2;
	int     numPts = xcStart->numPts;

	numPts++;  // due to last point replication

	ptsStart = xcStart->pts;
	ptsEnd = xcEnd->pts;
	pts1 = pts;
	pts2 = pts + numPts;

	for (i = 0; i < (numPts - 1); i++, pts1++, pts2++) {
		// copy over x,y from each xc
		*((POINT2D*)pts1) = *ptsStart++;
		*((POINT2D*)pts2) = *ptsEnd++;
		// set z for each
		pts1->z = 0.0f;
		pts2->z = length;
	}

	// Replicate last point in each u-band
	*pts1 = *pts;
	*pts2 = *(pts + numPts);
}

/**************************************************************************\
* ProcessXCPrimLinear
*
* Processes a prim according to evaluator data
*
* - Only valid for colinear xc's (along z)
* - XC's may be identical (extrusion).  If not identical, may have
*   discontinuities at each end.
* - Converts 2D XC pts to 3D pts
*
\**************************************************************************/

void
EVAL::ProcessXCPrimLinear(XC* xcStart, XC* xcEnd, float length) {
	if (length <= 0.0f)
		// nuttin' to do
		return;

	// Build a vertex control net from 2 xcObj's a distance 'length' apart
	// this will displace the end xcObj a distance 'length' down the z-axis
	SetVertexCtrlPtsXCTranslate(pts, length, xcStart, xcEnd);

	Evaluate();
}

/**************************************************************************\
* ProcessXCPrimBendSimple
*
* Processes a prim by bending along dir from xcCur
*
* - dir is relative from xc in x-y plane
* - adds C2 continuity at ends
*
\**************************************************************************/

void
EVAL::ProcessXCPrimBendSimple(XC* xcCur, int dir, float radius) {
	POINT3D* ptsSrc, * ptsDst;
	static float acPts[MAX_XC_PTS + 1];
	int ptSetStride = xcCur->numPts + 1; // pt stride for output pts buffer

	// We will be creating 4 cross-sectional control point sets here.

	// Convert 2D pts in xcCur to 3D pts at z=0 for 1st point set
	xcCur->ConvertPtsZ(pts, 0.0f);

	// Calc 4th point set by rotating 1st set as per dir
	ptsDst = pts + 3 * ptSetStride;
	RotatePointSet(pts, ptSetStride, 90.0f, dir, radius, ptsDst);

	// angles != 90, hard, cuz not easy to extrude 3rd set from 4th

	// Next, have to figure out ac values.  Need to extend each xc's points
	// into bend to generate ac net.  For circular bend (and later for general
	// case elliptical bend), need to know ac distance from xc for each point.
	// This is based on the point's turn radius - a function of its distance
	// from the 'hinge' of the turn.

	// Can take advantage of symmetry here.  Figure for one xc, good for 2nd.
	// This assumes 90 deg turn.  (also,last point replicated)
	xcCur->CalcArcACValues90(dir, radius, acPts);

	// 2) extrude each point's ac from xcCur (extrusion in +z)
	// apply values to 1st to get 2nd
	// MINUS_Z, cuz subtracts *back* from dir
	ExtrudePointSetDir(pts, ptSetStride, acPts, MINUS_Z,
													pts + ptSetStride);

	// 3) extrude each point's ac from xcEnd (extrusion in -dir)
	ptsSrc = pts + 3 * ptSetStride;
	ptsDst = pts + 2 * ptSetStride;
	ExtrudePointSetDir(ptsSrc, ptSetStride, acPts, dir, ptsDst);

	Evaluate();
}

/**************************************************************************\
* eval_ProcessXCPrimSingularity
*
* Processes a prim by joining singularity to an xc
*
* - Used for closing or opening the pipe
* - If bOpening is true, starts with singularity, otherwise ends with one
* - the xc side is always in z=0 plane
* - singularity side is radius on either side of xc
* - adds C2 continuity at ends (perpendicular to +z at singularity end)
*
\**************************************************************************/

void
EVAL::ProcessXCPrimSingularity(XC* xcCur, float length, BOOL bOpening) {
	POINT3D* ptsSing, * ptsXC;
	static float acPts[MAX_XC_PTS + 1];
	float zSing; // z-value at singularity
	int ptSetStride = xcCur->numPts + 1; // pt stride for output pts buffer
	int i;
	XC xcSing(xcCur);

	// create singularity xc - which is an extremely scaled-down version
	//  of xcCur (this prevents any end-artifacts, unless of course we were
	//  to zoom it ultra-large).

	xcSing.Scale(.0005f);

	// We will be creating 4 cross-sectional control point sets here.

	if (bOpening) {
		ptsSing = pts;
		ptsXC = pts + 3 * ptSetStride;
	} else {
		ptsSing = pts + 3 * ptSetStride;
		ptsXC = pts;
	}

	// Convert 2D pts in xcCur to 3D pts at 'xc' point set
	xcCur->ConvertPtsZ(ptsXC, 0.0f);

	// Set z-value for singularity point set
	zSing = bOpening ? -length : length;
	xcSing.ConvertPtsZ(ptsSing, zSing);

	// The arc control for each point is based on a radius value that is
	//  each xc point's distance from the xc center
	xcCur->CalcArcACValuesByDistance(acPts);

	// Calculate point set near xc
	if (bOpening)
		ExtrudePointSetDir(ptsXC, ptSetStride, acPts, PLUS_Z,
													ptsXC - ptSetStride);
	else
		ExtrudePointSetDir(ptsXC, ptSetStride, acPts, MINUS_Z,
													ptsXC + ptSetStride);

	// Point set near singularity is harder, as the points must generate
	// a curve between the singularity and each xc point
	// No, easier, just scale each point by universal arc controller !
	POINT3D* ptsDst = pts;
	ptsDst = bOpening ? ptsSing + ptSetStride : ptsSing - ptSetStride;
	for (i = 0; i < ptSetStride; i++, ptsDst++) {
		ptsDst->x = EVAL_CIRC_ARC_CONTROL * ptsXC[i].x;
		ptsDst->y = EVAL_CIRC_ARC_CONTROL * ptsXC[i].y;
		ptsDst->z = zSing;
	}

	Evaluate();
}

/**************************************************************************\
* Evaluate
*
* Evaluates the EVAL object
*
* - There may be 1 or more lengthwise sections around an xc
* - u is minor, v major
* - u,t run around circumference, v,s lengthwise
* - Texture maps are 2x2 for each section
* - ! uDiv is per section !
*
\**************************************************************************/

void
EVAL::Evaluate() {
	int i;
	POINT3D* ppts = pts;
	TEX_POINT2D* ptexPts = texPts;
	// total # pts in cross-section:
	int xcPointCount = (uOrder - 1) * numSections + 1;

	for (i = 0; i < numSections; i++,
									   ppts += (uOrder - 1),
									   ptexPts += (TEX_ORDER * TEX_ORDER)) {

		// map texture coords

		if (bTexture) {
			glMap2f(GL_MAP2_TEXTURE_COORD_2,
					0.0f, 1.0f, TDIM, TEX_ORDER,
					0.0f, 1.0f, TEX_ORDER * TDIM, TEX_ORDER,
					(GLfloat*)ptexPts);
		}

		// map vertices

		glMap2f(GL_MAP2_VERTEX_3,
			   0.0f, 1.0f, VDIM, uOrder,
			   0.0f, 1.0f, xcPointCount * VDIM, vOrder,
			   (GLfloat*)ppts);

		// evaluate

		glMapGrid2f(uDiv, 0.0f, 1.0f, vDiv, 0.0f, 1.0f);
		glEvalMesh2(GL_FILL, 0, uDiv, 0, vDiv);
	}
}

#if EVAL_DBG
/**************************************************************************\
* DrawPoints
*
* draw control points
*
\**************************************************************************/
static
void DrawPoints(int num, POINT3D* pts) {
	GLint i;

	// draw green pts for now
	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(2);

	glBegin(GL_POINTS);
	for (i = 0; i < num; i++, pts++) {
		glVertex3fv((GLfloat*)pts);
	}
	glEnd();
}
#endif

/**************************************************************************\
* ExtrudePointSetDir
*
* Extrude a point set back from the current direction
*
* Generates C2 continuity at the supplied point set xc, by generating another
* point set back of the first, using supplied subtraction values.
*
\**************************************************************************/

static void
ExtrudePointSetDir(POINT3D* inPts, int numPts, float* acPts, int dir,
					POINT3D* outPts) {
	int i;
	float sign;
	int offset;

	switch (dir) {
		case PLUS_X:
			offset = 0;
			sign = -1.0f;
			break;
		case MINUS_X:
			offset = 0;
			sign = 1.0f;
			break;
		case PLUS_Y:
			offset = 1;
			sign = -1.0f;
			break;
		case MINUS_Y:
			offset = 1;
			sign = 1.0f;
			break;
		case PLUS_Z:
			offset = 2;
			sign = -1.0f;
			break;
		case MINUS_Z:
			offset = 2;
			sign = 1.0f;
			break;
	}

	for (i = 0; i < numPts; i++, inPts++, outPts++, acPts++) {
		*outPts = *inPts;
		((float*)outPts)[offset] = ((float*)inPts)[offset] + (sign * (*acPts));
	}
}

/**************************************************************************\
* RotatePointSet
*
* Rotate point set by angle, according to dir and radius
*
* - Put points in supplied outPts buffer
*
\**************************************************************************/

static void
RotatePointSet(POINT3D* inPts, int numPts, float angle, int dir,
					  float radius, POINT3D* outPts) {
	MATRIX matrix1, matrix2, matrix3;
	int i;
	POINT3D rot = {0.0f};
	POINT3D anchor = {0.0f};

	/* dir      rot
	   +x       90 y
	   -x       -90 y
	   +y       -90 x
	   -y       90 x
	*/

	// convert angle to radians
	//mf: as noted in objects.c, we have to take negative angle to make
	// it work in familiar 'CCW rotation is positive' mode.  The ss_* rotate
	// routines must work in the 'CW is +'ve' mode, as axis pointing at you.
	angle = SS_DEG_TO_RAD(-angle);

	// set axis rotation and anchor point

	switch (dir) {
		case PLUS_X:
			rot.y = angle;
			anchor.x = radius;
			break;
		case MINUS_X:
			rot.y = -angle;
			anchor.x = -radius;
			break;
		case PLUS_Y:
			rot.x = -angle;
			anchor.y = radius;
			break;
		case MINUS_Y:
			rot.x = angle;
			anchor.y = -radius;
			break;
	}

	// translate anchor point to origin
	ss_matrixIdent(&matrix1);
	ss_matrixTranslate(&matrix1, -anchor.x, -anchor.y, -anchor.z);

	// rotate 
	ss_matrixIdent(&matrix2);
	ss_matrixRotate(&matrix2, (double)rot.x, rot.y, rot.z);

	// concat these 2
	ss_matrixMult(&matrix3, &matrix2, &matrix1);

	// translate back
	ss_matrixIdent(&matrix2);
	ss_matrixTranslate(&matrix2, anchor.x, anchor.y, anchor.z);

	// concat these 2
	ss_matrixMult(&matrix1, &matrix2, &matrix3);

	for (i = 0; i < numPts; i++, outPts++, inPts++) {
		ss_xformPoint(outPts, inPts, &matrix1);
	}
}





/******************************Public*Routine******************************\
* VIEW constructor
\**************************************************************************/

VIEW::VIEW() {
	bProjMode = GL_TRUE;

	// set some initial viewing and size params

	zTrans = -75.0f;
	viewDist = -zTrans;

	numDiv = NUM_DIV;
	SS_ASSERT(numDiv >= 2, "VIEW constructor: not enough divisions\n");
	// Because number of nodes in a dimension is derived from (numDiv-1), and
	// can't be 0

	divSize = 7.0f;

	persp.viewAngle = 90.0f;
	persp.zNear = 1.0f;

	yRot = 0.0f;

	winSize.width = winSize.height = 0;
}

/******************************Public*Routine******************************\
* SetProjMatrix
*
* Set GL view parameters
\**************************************************************************/

void
VIEW::SetGLView() {
	glViewport(0, 0, winSize.width, winSize.height);
	SetProjMatrix();
}

/******************************Public*Routine******************************\
* SetProjMatrix
*
* Set Projection matrix
\**************************************************************************/

void
VIEW::SetProjMatrix() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	persp.zFar = viewDist + world.z * 2;
	if (bProjMode) {
		gluPerspective(persp.viewAngle,
						aspectRatio,
						persp.zNear, persp.zFar);
	} else {
		glOrtho(-world.x / 2, world.x / 2, -world.y / 2, world.y / 2,
						  -world.z, world.z);
	}
	glMatrixMode(GL_MODELVIEW);
}

/******************************Public*Routine******************************\
* CalcNodeArraySize
*
* Based on the viewing width and height, and numDiv, calculate the x,y,z array
* node dimensions.
*
\**************************************************************************/

void
VIEW::CalcNodeArraySize(IPOINT3D* pNodeDim) {
	// if aspect ratio deviates too much from 1, then nodes will get
	// clipped as view rotates

	if (winSize.width >= winSize.height) {
		pNodeDim->x = numDiv - 1;
		pNodeDim->y = (int)(pNodeDim->x / aspectRatio);
		if (pNodeDim->y < 1)
			pNodeDim->y = 1;
		pNodeDim->z = pNodeDim->x;
	} else {
		pNodeDim->y = numDiv - 1;
		pNodeDim->x = (int)(aspectRatio * pNodeDim->y);
		if (pNodeDim->x < 1)
			pNodeDim->x = 1;
		pNodeDim->z = pNodeDim->y;
	}
}

/******************************Public*Routine******************************\
* SetWinSize
*
* Set the window size for the view, derive other view params.
*
* Return FALSE if new size same as old.
\**************************************************************************/

BOOL
VIEW::SetWinSize(int width, int height) {
	if ((width == winSize.width) &&
		(height == winSize.height))
		return FALSE;

	winSize.width = width;
	winSize.height = height;

	aspectRatio = winSize.height == 0 ? 1.0f : (float)winSize.width / winSize.height;

	if (winSize.width >= winSize.height) {
		world.x = numDiv * divSize;
		world.y = world.x / aspectRatio;
		world.z = world.x;
	} else {
		world.y = numDiv * divSize;
		world.x = world.y * aspectRatio;
		world.z = world.y;
	}
	return TRUE;
}

/******************************Public*Routine******************************\
* SetSceneRotation
*
\**************************************************************************/

void
VIEW::IncrementSceneRotation() {
	yRot += 9.73156f;
	if (yRot >= 360.0f)
		// prevent overflow
		yRot -= 360.0f;
}



/**************************************************************************\
* XC::CalcArcACValues90
*
* Calculate arc control points for a 90 degree rotation of an xc
*
* Arc is a quarter-circle
* - 90 degree is much easier, so we special case it
* - radius is distance from xc-origin to hinge of turn
*
\**************************************************************************/

void
XC::CalcArcACValues90(int dir, float radius, float* acPts) {
	int i;
	float sign;
	int offset;
	float* ppts = (float*)pts;

	// 1) calc 'r' values for each point (4 turn possibilities/point).  From
	//  this can determine ac, which is extrusion of point from xc face

	switch (dir) {
		case PLUS_X:
			offset = 0;
			sign = -1.0f;
			break;
		case MINUS_X:
			offset = 0;
			sign = 1.0f;
			break;
		case PLUS_Y:
			offset = 1;
			sign = -1.0f;
			break;
		case MINUS_Y:
			offset = 1;
			sign = 1.0f;
			break;
	}

	for (i = 0; i < numPts; i++, ppts += 2, acPts++) {
		*acPts = EVAL_CIRC_ARC_CONTROL * (radius + (sign * ppts[offset]));
	}
	// replicate !
	*acPts = *(acPts - numPts);
}

/**************************************************************************\
* XC::CalcArcACValuesByDistance
*
* Use the distance of each xc point from the xc origin, as the radius for
* an arc control value.
*
\**************************************************************************/

void
XC::CalcArcACValuesByDistance(float* acPts) {
	int i;
	float r;
	POINT2D* ppts = pts;

	for (i = 0; i < numPts; i++, ppts++) {
		r = (float)sqrt(ppts->x * ppts->x + ppts->y * ppts->y);
		*acPts++ = EVAL_CIRC_ARC_CONTROL * r;
	}
	// replicate !
	*acPts = *(acPts - numPts);
}

/**************************************************************************\
* ELLIPTICAL_XC::SetControlPoints
*
* Set the 12 control points for a circle at origin in z=0 plane
*
* Points go CCW from +x
*
\**************************************************************************/

void
ELLIPTICAL_XC::SetControlPoints(GLfloat r1, GLfloat r2) {
	GLfloat ac1, ac2;

	ac1 = EVAL_CIRC_ARC_CONTROL * r2;
	ac2 = EVAL_CIRC_ARC_CONTROL * r1;

	// create 12-pt. set CCW from +x

	// last 2 points of right triplet
	pts[0].x = r1;
	pts[0].y = 0.0f;
	pts[1].x = r1;
	pts[1].y = ac1;

	// top triplet
	pts[2].x = ac2;
	pts[2].y = r2;
	pts[3].x = 0.0f;
	pts[3].y = r2;
	pts[4].x = -ac2;
	pts[4].y = r2;

	// left triplet
	pts[5].x = -r1;
	pts[5].y = ac1;
	pts[6].x = -r1;
	pts[6].y = 0.0f;
	pts[7].x = -r1;
	pts[7].y = -ac1;

	// bottom triplet
	pts[8].x = -ac2;
	pts[8].y = -r2;
	pts[9].x = 0.0f;
	pts[9].y = -r2;
	pts[10].x = ac2;
	pts[10].y = -r2;

	// first point of first triplet
	pts[11].x = r1;
	pts[11].y = -ac1;
}

/**************************************************************************\
* RANDOM4ARC_XC::SetControlPoints
*
* Set random control points for xc
* Points go CCW from +x
*
\**************************************************************************/

void
RANDOM4ARC_XC::SetControlPoints(float radius) {
	int i;
	GLfloat r[4];
	float rMin = 0.5f * radius;
	float distx, disty;

	// figure the radius of each side first

	for (i = 0; i < 4; i++)
		r[i] = ss_fRand(rMin, radius);

	// The 4 r's now describe a box around the origin - this restricts stuff

	// Now need to select a point along each edge of the box as the joining
	// points for each arc (join points are at indices 0,3,6,9)

	pts[0].x = r[RIGHT];
	pts[3].y = r[TOP];
	pts[6].x = -r[LEFT];
	pts[9].y = -r[BOTTOM];

	// quarter of distance between edges
	disty = (r[TOP] - -r[BOTTOM]) / 4.0f;
	distx = (r[RIGHT] - -r[LEFT]) / 4.0f;

	// uh, put'em somwhere in the middle half of each side
	pts[0].y = ss_fRand(-r[BOTTOM] + disty, r[TOP] - disty);
	pts[6].y = ss_fRand(-r[BOTTOM] + disty, r[TOP] - disty);
	pts[3].x = ss_fRand(-r[LEFT] + distx, r[RIGHT] - distx);
	pts[9].x = ss_fRand(-r[LEFT] + distx, r[RIGHT] - distx);

	// now can calc ac's
	// easy part first:
	pts[1].x = pts[11].x = pts[0].x;
	pts[2].y = pts[4].y = pts[3].y;
	pts[5].x = pts[7].x = pts[6].x;
	pts[8].y = pts[10].y = pts[9].y;

	// right side ac's
	disty = (r[TOP] - pts[0].y) / 4.0f;
	pts[1].y = ss_fRand(pts[0].y + disty, r[TOP]);
	disty = (pts[0].y - -r[BOTTOM]) / 4.0f;
	pts[11].y = ss_fRand(-r[BOTTOM], pts[0].y - disty);

	// left side ac's
	disty = (r[TOP] - pts[6].y) / 4.0f;
	pts[5].y = ss_fRand(pts[6].y + disty, r[TOP]);
	disty = (pts[6].y - -r[BOTTOM]) / 4.0f;
	pts[7].y = ss_fRand(-r[BOTTOM], pts[6].y - disty);

	// top ac's
	distx = (r[RIGHT] - pts[3].x) / 4.0f;
	pts[2].x = ss_fRand(pts[3].x + distx, r[RIGHT]);
	distx = (pts[3].x - -r[LEFT]) / 4.0f;
	pts[4].x = ss_fRand(-r[LEFT], pts[3].x - distx);

	// bottom ac's
	distx = (r[RIGHT] - pts[9].x) / 4.0f;
	pts[10].x = ss_fRand(pts[9].x + distx, r[RIGHT]);
	distx = (pts[9].x - -r[LEFT]) / 4.0f;
	pts[8].x = ss_fRand(-r[LEFT], pts[9].x - distx);
}


/**************************************************************************\
* ConvertPtsZ
*
* Convert the 2D pts in an xc, to 3D pts in point buffer, with z.
*
* Also replicate the last point.
\**************************************************************************/

void
XC::ConvertPtsZ(POINT3D* newpts, float z) {
	int i;
	POINT2D* xcPts = pts;

	for (i = 0; i < numPts; i++, newpts++) {
		*((POINT2D*)newpts) = *xcPts++;
		newpts->z = z;
	}
	*newpts = *(newpts - numPts);
}

/**************************************************************************\
* XC::CalcBoundingBox
*
* Calculate bounding box in x/y plane for xc
\**************************************************************************/


void
XC::CalcBoundingBox() {
	POINT2D* ppts = pts;
	int i;
	float xMin, xMax, yMax, yMin;

	// initialize to really insane numbers
	xMax = yMax = -FLT_MAX;
	xMin = yMin = FLT_MAX;

	// compare with rest of points
	for (i = 0; i < numPts; i++, ppts++) {
		if (ppts->x < xMin)
			xMin = ppts->x;
		else if (ppts->x > xMax)
			xMax = ppts->x;
		if (ppts->y < yMin)
			yMin = ppts->y;
		else if (ppts->y > yMax)
			yMax = ppts->y;
	}
	xLeft = xMin;
	xRight = xMax;
	yBottom = yMin;
	yTop = yMax;
}

/**************************************************************************\
*
* MinTurnRadius
*
* Get minimum radius for the xc to turn in given direction.
*
* If the turn radius is less than this minimum, then primitive will 'fold'
* over itself at the inside of the turn, creating ugliness.
*
\**************************************************************************/

float
XC::MinTurnRadius(int relDir) {
// for now, assume xRight, yTop positive, xLeft, yBottom negative
// otherwise, might want to consider 'negative'radius
	switch (relDir) {
		case PLUS_X:
			return(xRight);
		case MINUS_X:
			return(-xLeft);
		case PLUS_Y:
			return(yTop);
		case MINUS_Y:
			return(-yBottom);
		default:
			return(0.0f);
	}
}
/**************************************************************************\
*
* XC::MaxExtent
*
* Get maximum extent of the xc in x and y
*
\**************************************************************************/

float
XC::MaxExtent() {
	float max;

	max = xRight;

	if (yTop > max)
		max = yTop;
	if (-xLeft > max)
		max = -xLeft;
	if (-yBottom > max)
		max = -yBottom;

	return max;
}

/**************************************************************************\
*
* XC::Scale
*
* Scale an XC's points and extents by supplied scale value
*
\**************************************************************************/

void
XC::Scale(float scale) {
	int i;
	POINT2D* ppts = pts;

	for (i = 0; i < numPts; i++, ppts++) {
		ppts->x *= scale;
		ppts->y *= scale;
	}

	xLeft *= scale;
	xRight *= scale;
	yBottom *= scale;
	yTop *= scale;
}

/**************************************************************************\
* ~XC::XC
*
* Destructor
*
\**************************************************************************/

XC::~XC() {
	if (pts)
		LocalFree(pts);
}

/**************************************************************************\
* XC::XC
*
* Constructor
*
* - Allocates point buffer for the xc
*
\**************************************************************************/

XC::XC(int nPts) {
	numPts = nPts;
	pts = (POINT2D*)LocalAlloc(LMEM_FIXED, numPts * sizeof(POINT2D));
	SS_ASSERT(pts != 0, "XC constructor\n");
}


XC::XC(XC* xc) {
	numPts = xc->numPts;
	pts = (POINT2D*)LocalAlloc(LMEM_FIXED, numPts * sizeof(POINT2D));
	SS_ASSERT(pts != 0, "XC constructor\n");
	RtlCopyMemory(pts, xc->pts, numPts * sizeof(POINT2D));

	xLeft = xc->xLeft;
	xRight = xc->xRight;
	yBottom = xc->yBottom;
	yTop = xc->yTop;
}

/**************************************************************************\
*
* ELLIPTICAL_XC::ELLIPTICALXC
*
* Elliptical XC constructor

* These have 4 sections of 4 pts each, with pts shared between sections.
*
\**************************************************************************/

ELLIPTICAL_XC::ELLIPTICAL_XC(float r1, float r2)
	// initialize base XC with numPts
	: XC((int)EVAL_XC_CIRC_SECTION_COUNT* (EVAL_ARC_ORDER - 1)) {
	SetControlPoints(r1, r2);
	CalcBoundingBox();
}

/**************************************************************************\
*
* RANDOM4ARC_XC::RANDOM4ARC_XC
*
* Random 4-arc XC constructor

* The bounding box is 2*r each side
* These have 4 sections of 4 pts each, with pts shared between sections.
*
\**************************************************************************/
RANDOM4ARC_XC::RANDOM4ARC_XC(float r)
	// initialize base XC with numPts
	: XC((int)EVAL_XC_CIRC_SECTION_COUNT* (EVAL_ARC_ORDER - 1)) {
	SetControlPoints(r);
	CalcBoundingBox();
}

