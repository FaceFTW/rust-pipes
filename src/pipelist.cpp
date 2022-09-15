#include "include/pipe_layer.h"
#include <iterator>

using namespace GlPipes;

/******CONSTRUCTORS/DESTRUCTORS*******/
PipeList::PipeList(int numPipes) {
	pipeCnt = numPipes;
	internal = new std::vector<Point>*[numPipes];
	for(int i = 0; i < numPipes; i++) { internal[i] = new std::vector<Point>(); }
}

PipeList::~PipeList() {
	for(int i = 0; i < pipeCnt; i++) {
		for(int j = 0; j < internal[i]->size(); j++) { delete internal[i]; }
		delete[] internal;
	}
}

/******OVERLOADED OPERATORS*******/
std::vector<Point>*& PipeList::operator[](int pipeIdx) { return internal[pipeIdx]; }

Point& PipeList::operator()(int pipeIdx, int idx) { return internal[pipeIdx]->at(idx); }

/******PUBLIC METHODS*******/
void PipeList::addToPipe(int pipeIdx, Point* point) { internal[pipeIdx]->push_back(point); }

void PipeList::outputPipePath(int pipe) {
	auto iter = std::begin(*(internal[pipe]));
	auto end = std::end(*(internal[pipe]));

	while(iter != end) {
		std::cout << *iter << std::endl;
		iter++;
	}
}