/**
 * @file gl_init.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-03-18
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include "../include/graphics/gl_init.h"

void initGLMisc() {
	static float ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
	static float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	static float position[] = {90.0f, 90.0f, 150.0f, 0.0f};
	static float lmodel_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	static float lmodel_ambientTex[] = {0.6f, 0.6f, 0.6f, 0.0f};
	static float back_mat_diffuse[] = {0.0f, 0.0f, 1.0f};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CCW);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_AUTO_NORMAL);// needed for GL_MAP2_VERTEX (tea)

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
}

void initGLView(GLConfig* cfg) {
	glViewport(0, 0, cfg->winWidth, cfg->winHeight);

	//Set Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	cfg->zFar = (cfg->viewDist) + (cfg->worldz) * 2;
	// if(bProjMode) {
	gluPerspective(cfg->viewAngle,
	               cfg->aspectRatio,
	               cfg->zNear, cfg->zFar);
	// } else {
	// 	glOrtho(-cfg->worldx / 2, cfg->worldx / 2, -cfg->worldy / 2, cfg->worldy / 2,
	// 	        -cfg->worldz, cfg->worldz);
	// }
	glMatrixMode(GL_MODELVIEW);
}

bool doViewMath(GLConfig* cfg, int width, int height) {
	if((width == cfg->winWidth) &&
	   (height == cfg->winHeight)) {
		return false;
	}

	cfg->winWidth = width;
	cfg->winHeight = height;

	cfg->aspectRatio = cfg->winHeight == 0 ? 1.0f : (float) cfg->winWidth / cfg->winHeight;

	if(cfg->winWidth >= cfg->winHeight) {
		cfg->worldx = cfg->numDiv * cfg->divSize;
		cfg->worldy = cfg->worldx / cfg->aspectRatio;
		cfg->worldz = cfg->worldx;
	} else {
		cfg->worldy = cfg->numDiv * cfg->divSize;
		cfg->worldx = cfg->worldy * cfg->aspectRatio;
		cfg->worldz = cfg->worldy;
	}

	return true;
}

void incrementSceneRotation(GLConfig* cfg) {
	cfg->yRot += 9.73156f;
	if(cfg->yRot >= 360.0f)
		// prevent overflow
		cfg->yRot -= 360.0f;
}

void calcNodeArraySize(GLConfig* cfg, IPOINT3D* pNodeDim) {
	// if aspect ratio deviates too much from 1, then nodes will get
	// clipped as view rotates

	if(cfg->winWidth >= cfg->winHeight) {
		pNodeDim->x = cfg->numDiv - 1;
		pNodeDim->y = (int) (pNodeDim->x / cfg->aspectRatio);
		if(pNodeDim->y < 1)
			pNodeDim->y = 1;
		pNodeDim->z = pNodeDim->x;
	} else {
		pNodeDim->y = cfg->numDiv - 1;
		pNodeDim->x = (int) (cfg->aspectRatio * pNodeDim->y);
		if(pNodeDim->x < 1)
			pNodeDim->x = 1;
		pNodeDim->z = pNodeDim->y;
	}
}
