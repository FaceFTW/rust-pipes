/**
 * @file gl_drawThread.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#include "../include/graphics/gl_drawThread.h"

DRAW_THREAD::DRAW_THREAD() {
	pPipe = nullptr;
}

DRAW_THREAD::~DRAW_THREAD() {
}

void DRAW_THREAD::SetPipe(PIPE* pipe) {
	pPipe = pipe;
}

void DRAW_THREAD::DrawPipe() {
	((PIPE*) pPipe)->Draw();
	glFlush();
}

bool DRAW_THREAD::StartPipe() {
	((PIPE*) pPipe)->Start();
	glFlush();

	// check status
	if(pPipe->NowhereToRun())
		return false;
	else
		return true;
}

void DRAW_THREAD::KillPipe() {
	delete(PIPE*) pPipe;
	pPipe = nullptr;
}
