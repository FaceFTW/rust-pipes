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

#include "../include/global_state.h"

DrawThread::DrawThread() {
	pPipe = nullptr;
}

DrawThread::~DrawThread() {
}

void DrawThread::setPipe(Pipe* pipe) {
	pPipe = pipe;
}

void DrawThread::drawPipe() {
	((Pipe*) pPipe)->draw();
	glFlush();
}

bool DrawThread::startPipe() {
	((Pipe*) pPipe)->start();
	glFlush();

	// check status
	if(pPipe->nowhereToRun())
		return false;
	else
		return true;
}

void DrawThread::killPipe() {
	delete(Pipe*) pPipe;
	pPipe = nullptr;
}
