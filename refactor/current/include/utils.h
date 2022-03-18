/**
 * @file utils.h
 * @author Alex "FaceFTW" Westerman
 * @brief A Bunch of Utility Methods that made sense to port from the original
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include <GL/gl.h>
#include "structs.h"

#ifndef __utils_h_
#define __utils_h_
// Maximum texture bitmap dimensions.

#define TEX_WIDTH_MAX   1280
#define TEX_HEIGHT_MAX  1024

#define PI 3.14159265358979323846f
// double version of PI
#define PI_D 3.14159265358979323846264338327950288419716939937510
#define ONE_OVER_PI (1.0f / PI)
#define ROOT_TWO 1.414213562373f

#define GEN_STRING_SIZE 64

#ifndef GL_EXT_paletted_texture
#define GL_COLOR_INDEX1_EXT                   0x80E2
#define GL_COLOR_INDEX2_EXT                   0x80E3
#define GL_COLOR_INDEX4_EXT                   0x80E4
#define GL_COLOR_INDEX8_EXT                   0x80E5
#define GL_COLOR_INDEX12_EXT                  0x80E6
#define GL_COLOR_INDEX16_EXT                  0x80E7
typedef void (APIENTRY* PFNGLCOLORTABLEEXTPROC)
(GLenum target, GLenum internalFormat, GLsizei width, GLenum format,
 GLenum type, const GLvoid* data);
typedef void (APIENTRY* PFNGLCOLORSUBTABLEEXTPROC)
(GLenum target, GLsizei start, GLsizei count, GLenum format,
 GLenum type, GLvoid* data);
#endif

// texture resource

#define RT_RGB          99
#define RT_MYBMP        100
#define RT_A8           101

// Useful macros

#define UTIL_MAX( a, b ) \
    ( a > b ? a : b )

#define UTIL_MIN( a, b ) \
    ( a < b ? a : b )

// macro to round up floating values
#define UTIL_ROUND_UP( fval ) \
    ( (((fval) - (float)(int)(fval)) > 0.0f) ? (int) ((fval)+1.0f) : (int) (fval) )

// macros to clamp a value within a range
#define UTIL_CLAMP_TO_RANGE( a, lo, hi ) ( (a < lo) ? lo : ((a > hi) ? hi : a) )
#define UTIL_CLAMP_TO_RANGE2( a, lo, hi ) \
    ( a = (a < lo) ? lo : ((a > hi) ? hi : a) )

// degree<->radian macros
#define ONE_OVER_180 (1.0f / 180.0f)
#define UTIL_DEG_TO_RAD( a ) ( (a*PI) * ONE_OVER_180 )
#define UTIL_RAD_TO_DEG( a ) ( (a*180.0f) * ONE_OVER_PI )


/**
 * @brief Generates integer random number 0..(max-1)
 *
 * @param max
 * @return int
 */
extern int iRand(int max);

/**
 * @brief Generates integer random number min..max
 *
 * @param min
 * @param max
 * @return int
 */
extern int iRand2(int min, int max);

/**
 * @brief Generates float random number min...max
 *
 * @param min
 * @param max
 * @return int
 */
extern float fRand(float min, float max);

extern POINT3D ss_ptZero;
extern void xformPoint(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
extern void xformNorm(POINT3D* ptOut, POINT3D* ptIn, MATRIX*);
extern void matrixIdent(MATRIX*);
extern void matrixRotate(MATRIX* m, double xTheta, double yTheta, double zTheta);
extern void matrixTranslate(MATRIX*, double xTrans, double yTrans, double zTrans);
extern void matrixMult(MATRIX* m1, MATRIX* m2, MATRIX* m3);
extern void calcNorm(POINT3D* norm, POINT3D* p1, POINT3D* p2, POINT3D* p3);
extern void normalizeNorm(POINT3D*);
extern void normalizeNorms(POINT3D*, unsigned long);
#endif //__utils_h_