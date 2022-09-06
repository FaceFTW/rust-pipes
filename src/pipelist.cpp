#include "include/pipe_layer.h"

using namespace GlPipes;

/******CONSTRUCTORS/DESTRUCTORS*******/
PipeList::PipeList(int numPipes) {
	internal = new std::vector<std::vector<Point*>*>();
	for(int i = 0; i < numPipes; i++) { internal->push_back(new std::vector<Point*>()); }
}

PipeList::~PipeList() {
	for(int i = 0; i < internal->size(); i++) {
		for(int j = 0; j < internal->at(i)->size(); j++) { delete internal->at(i)->at(j); }
		delete internal->at(i);
	}
	delete internal;
}

/******OVERLOADED OPERATORS*******/
std::vector<Point*>*& PipeList::operator[](int pipeIdx) { return internal->at(pipeIdx); }

Point*& PipeList::operator()(int pipeIdx, int idx) { return internal->at(pipeIdx)->at(idx); }

/******PUBLIC METHODS*******/
void PipeList::addToPipe(int pipeIdx, Point* point) { internal->at(pipeIdx)->push_back(point); }
