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

#ifdef __cplusplus
extern "C" {
#endif

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
	((((fval) - (FLOAT) (int) (fval)) > 0.0f) ? (int) ((fval) + 1.0f) : (int) (fval))

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

#ifdef __cplusplus
}
#endif

#endif//__utils_h_