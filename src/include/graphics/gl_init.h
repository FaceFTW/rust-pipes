/**
 * @file gl_init.h
 * @author Alex "FaceFTW" Westerman
 * @brief Functions for initializing OpenGL
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include <GL/gl.h>
#include <GL/glu.h>

#include "../constants.h"
#include "../structs.h"

#ifndef __gl_init_h_
#define __gl_init_h_

/**
 * @brief A general init function to set the GL scene up
 * *this was copied from the original program's state.cpp, sans texture stuff*
 */
void initGLMisc();

/**
 * @brief Init function configuring the GL View
 * *this was copied from the original program's view.cpp*
 */
void initGLView(GLConfig* cfg);

/**
 * @brief Does some math for configuring the view with the specified dimensions
 * 
 * @param cfg Config Struct containing the data we want
 * @param width The new width of the view
 * @param height The new height of the view
 * @return true If the view dimensions were updated
 * @return false If the view dimensions were the same
 */
bool doViewMath(GLConfig* cfg, int width, int height);

void incrementSceneRotation(GLConfig* cfg);

void calcNodeArraySize(GLConfig* cfg, IPOINT3D* pNodeDim);
#endif//__gl_init_h_