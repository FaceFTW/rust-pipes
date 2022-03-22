/**
 * @file utils.h
 * @author Alex "FaceFTW" Westerman
 * @brief Utility Functions/Defines/Macros
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#ifndef __utils_h_
#define __utils_h_

// #ifdef __cplusplus
// extern "C" {
// #endif

#include "structs.h"

/**
 * @brief Returns the maximum of two values
 */
#define MAX(a, b) (a > b ? a : b)

/**
 * @brief Returns the minimum of two values
 */
#define MIN(a, b) (a < b ? a : b)

// macro to round up floating values
#define ROUND_UP(fval) \
	((((fval) - (float) (int) (fval)) > 0.0f) ? (int) ((fval) + 1.0f) : (int) (fval))

// macros to clamp a value within a range
#define CLAMP_TO_RANGE(a, lo, hi) ((a < lo) ? lo : ((a > hi) ? hi : a))
#define CLAMP_TO_RANGE2(a, lo, hi) (a = (a < lo) ? lo : ((a > hi) ? hi : a))

// degree<->radian macros
#define ONE_OVER_180 (1.0f / 180.0f)
#define SS_DEG_TO_RAD(a) ((a * PI) * ONE_OVER_180)
#define SS_RAD_TO_DEG(a) ((a * 180.0f) * ONE_OVER_PI)

//extern void RandInit( void );

/**
 * @brief Generates integer random number
 * 
 * @param max Maximum number 
 * @return int  Random number between 0 and (max-1)
 */
int iRand(int max);

/**
 * @brief Generates integer random number
 * 
 * @param min Minimum of Random
 * @param max Maximum of Random
 * @return int Random integer between min and max
 */
int iRand2(int min, int max);

/**
 * @brief Generates float random number
 * 
 * @param min Minimum of Random
 * @param max Maximum of Random
 * @return float Random float between min and max
 */
float fRand(float min, float max);

/**
 * @brief rotate circle around x-axis, with edge attached to anchor
 * @param angle 
 * @param inPoint 
 * @param outPoint 
 * @param num 
 * @param anchor 
 */
void transformCircle(float angle,
                     POINT3D* inPoint,
                     POINT3D* outPoint,
                     GLint num,
                     POINT3D* anchor);

/**
 * @brief 
 * 
 * @param p 
 * @param n 
 * @param center 
 * @param num 
 */
void calcNormals(POINT3D* p,
                 POINT3D* n,
                 POINT3D* center,
                 int num);

extern POINT3D ss_ptZero;
void xformPoint(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
void xformNorm(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
void matrixIdent(MATRIX*);
void matrixRotate(MATRIX* m, double xTheta, double yTheta, double zTheta);
void matrixTranslate(MATRIX*, double xTrans, double yTrans, double zTrans);
void matrixMult(MATRIX* m1, MATRIX* m2, MATRIX* m3);
void calcNorm(POINT3D* norm, POINT3D* p1, POINT3D* p2, POINT3D* p3);
void normalizeNorm(POINT3D*);
void normalizeNorms(POINT3D*, unsigned long);

void align_notch(int newDir, int notch);
// static GLint defCylNotch[NUM_DIRS];
// static GLfloat alignNotchRot[NUM_DIRS][NUM_DIRS];
void align_plusy(int oldDir, int newDir);
// static float RotZ[NUM_DIRS][NUM_DIRS];

/**
 * @brief Aligns the z axis along specified direction
 *  Used for all types of pipes
 * @param newDir 
 */
void align_plusz(int newDir);

/**
 * @brief This array tells you which way the notch will be once you make a turn
 * Format: notchTurn[oldDir][newDir][notchVec] 
 * 
 */
// static GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS];

// #ifdef __cplusplus
// }
// #endif

#endif//__utils_h_