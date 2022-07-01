/**
 * @file node_struct.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-06-27
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
#pragma once
#include "include/node_struct.h"
#include "include/utils.h"
#include <cstddef>

using namespace GlPipes;

/******CONSTRUCTORS/DESTRUCTORS*******/
template<class T> NodeStruct<T>::NodeStruct() {
	node_struct_size = new Point(10, 10, 10);
	node_struct = new T***[node_struct_size->x];

	for(int i = 0; i < node_struct_size->x; i++) {
		node_struct[i] = new T**[node_struct_size->y];
		for(int j = 0; j < node_struct_size->y; j++) {
			node_struct[i][j] = new T*[node_struct_size->z];
		}
	}
}

template<class T> NodeStruct<T>::NodeStruct(Point* size) {
	node_struct = new T***[size->x];
	node_struct_size = size;

	for(int i = 0; i < size->x; i++) {
		node_struct[i] = new T**[size->y];
		for(int j = 0; j < size->y; j++) { node_struct[i][j] = new T*[size->z]; }
	}
}

template<class T> NodeStruct<T>::~NodeStruct<T>() {
	//PERF Very inelegant, prob a better way of doing this, but still;
	for(int i = 0; i < node_struct_size->x; i++) {
		for(int j = 0; j < node_struct_size->y; j++) { delete[] node_struct[i][j]; }
		delete[] node_struct[i];
	}
	delete[] node_struct;
}

/******OVERLOADED OPERATORS*******/
template<class T> T& NodeStruct<T>::operator()(uint x, uint y, uint z) {
	return node_struct[x][y][z];
}

template<class T> T& NodeStruct<T>::operator[](Point* pos) {
	return node_struct[pos->x][pos->y][pos->z];
}

template<class T> bool NodeStruct<T>::isEmpty(Point* pos) {
	if(((pos->x >= 0) && (pos->x < node_struct->size())) &&
	   ((pos->y >= 0) && (pos->y < node_struct[pos->x]->size())) &&
	   ((pos->z >= 0) && (pos->z < node_struct[pos->x][pos->y]->size()))) {
		return node_struct[pos->x][pos->y][pos->z] != NULL;
	} else {
		//This is exception worthy, this should not happen
		throw;//TODO
	}
}

template<class T> Point* NodeStruct<T>::getNextNodePos(Point* curPos, Direction dir) {
	switch(dir) {
		case DIR_X_PLUS:
			if((curPos->x) + 1 < this->size(AXIS_X)) return new Point(curPos, DIR_X_PLUS);
			break;
		case DIR_X_MINUS:
			if((curPos->x) - 1 < 0) return new Point(curPos, DIR_X_MINUS);
			break;
		case DIR_Y_PLUS:
			if((curPos->y) + 1 < this->size(AXIS_Y)) return new Point(curPos, DIR_Y_PLUS);
			break;
		case DIR_Y_MINUS:
			if((curPos->y) - 1 < 0) return new Point(curPos, DIR_Y_MINUS);
			break;
		case DIR_Z_PLUS:
			if((curPos->z) + 1 < this->size(AXIS_Z)) return new Point(curPos, DIR_Z_PLUS);
			break;
		case DIR_Z_MINUS:
			if((curPos->z) - 1 < 0) return new Point(curPos, DIR_Z_MINUS);
			break;
		case DIR_NONE: break;
	}
	return nullptr;
}

template<class T> Point** NodeStruct<T>::getNeighbors(Point* pos) {
	Point** neighbors = new Point*[NUM_DIRS];
	neighbors[DIR_X_PLUS] = getNextNodePos(pos, DIR_X_PLUS);
	neighbors[DIR_X_MINUS] = getNextNodePos(pos, DIR_X_MINUS);
	neighbors[DIR_Y_PLUS] = getNextNodePos(pos, DIR_Y_PLUS);
	neighbors[DIR_Y_MINUS] = getNextNodePos(pos, DIR_Y_MINUS);
	neighbors[DIR_Z_PLUS] = getNextNodePos(pos, DIR_Z_PLUS);
	neighbors[DIR_Z_MINUS] = getNextNodePos(pos, DIR_Z_MINUS);
	return neighbors;
}

template<class T> int NodeStruct<T>::getEmptyNeighbors(Point* pos, Direction* emptyDirs) {
	int count = 0;

	for(int i = 0; i < NUM_DIRS; i++) {
		if(getNextNodePos(pos, i)) { emptyDirs[count++] = (Direction) i; }
	}
	return count;
}

template<class T>
int NodeStruct<T>::getEmptyTurnNeighbors(Point* pos, Direction* emptyDirs, Direction lastDir) {
	int count = 0;

	for(int i = 0; i < NUM_DIRS; i++) {
		if(getNextNodePos(pos, i)) {
			if(i == (int) lastDir) continue;
			emptyDirs[count++] = (Direction) i;
		}
	}
	return count;
}

template<class T>
int NodeStruct<T>::getEmptyNeighborsAlongDir(Point* pos, Direction dir, int searchRadius) {
	int nodeStride;
	int maxSearch;
	int count = 0;
	Point* curPos = new Point(pos);

	switch(dir) {
		case DIR_X_PLUS: maxSearch = size(AXIS_X) - pos->x - 1; break;
		case DIR_X_MINUS: maxSearch = pos->x; break;
		case DIR_Y_PLUS: maxSearch = size(AXIS_Y) - pos->y - 1; break;
		case DIR_Y_MINUS: maxSearch = pos->y; break;
		case DIR_Z_PLUS: maxSearch = size(AXIS_Z) - pos->z - 1; break;
		case DIR_Z_MINUS: maxSearch = pos->z; break;
	}

	if(searchRadius > maxSearch) searchRadius = maxSearch;

	if(!searchRadius) return 0;

	while(searchRadius--) {
		switch(dir) {
			case DIR_X_PLUS: (curPos->x)++; break;
			case DIR_X_MINUS: (curPos->x)--; break;
			case DIR_Y_PLUS: (curPos->y)++; break;
			case DIR_Y_MINUS: (curPos->y)--; break;
			case DIR_Z_PLUS: (curPos->z)++; break;
			case DIR_Z_MINUS: (curPos->z)--; break;
		}
		if(!this[curPos]) return count;
		count++;
	}
	return count;
}

template<class T>
Direction NodeStruct<T>::chooseRandomDirection(Point* pos, Direction dir, int weight) {
	Point** neighbors = getNeighbors(pos);
	int numEmpty, choice;
	Direction newDir;
	Point* straightNode = NULL;
	Direction* emptyDirs = new Direction;

	// Get node in straight direction if necessary
	if(weight && neighbors[dir] && isEmpty(neighbors[dir])) {
		straightNode = neighbors[dir];
		// if maximum weight, choose and return
		if(weight == MAX_WEIGHT_STRAIGHT) { return dir; }
	} else
		weight = 0;

	// Get directions of possible turns
	numEmpty = getEmptyTurnNeighbors(pos, emptyDirs, dir);

	// Make a random choice
	if((choice = (weight + numEmpty)) == 0) return DIR_NONE;
	choice = iRand(choice);

	if(choice < weight) {
		return dir;
	} else {
		// choose one of the turns
		newDir = emptyDirs[choice - weight];
		return newDir;
	}
}

template<class T> Direction NodeStruct<T>::choosePreferredDirection(Point* pos, Direction dir,
                                                                    Direction* prefDirs,
                                                                    int nPrefDirs) {
	Direction neighbors[NUM_DIRS];
	int numEmpty;
	Direction newDir;
	Direction emptyDirs[NUM_DIRS];
	Direction pEmptyPrefDirs[NUM_DIRS];
	int i, j;

	// Get the neigbouring nodes
	neighbors = getNeighbors(pos);

	// Create list of directions that are both preferred and empty

	// pEmptyPrefDirs = emptyDirs;F
	numEmpty = 0;

	for(i = 0, j = 0; (i < NUM_DIRS) && (j < nPrefDirs); i++) {
		if(i == prefDirs[i]) {
			if(neighbors[i] && isEmpty(neighbors[i])) {
				// add it to list
				pEmptyPrefDirs[j] = (Direction) i;
				numEmpty++;
				j++;
			}
		}
	}

	// if no empty preferred dirs, then any empty dirs become preferred

	if(!numEmpty) {
		numEmpty = getEmptyNeighbors(pos, emptyDirs);
		if(numEmpty == 0) return DIR_NONE;
	}

	// Pick a random dir from the empty set

	newDir = emptyDirs[iRand(numEmpty)];
	return newDir;
}

//TODO Finish after other methods completed
template<class T> Direction NodeStruct<T>::chooseNewTurnDirection(Point* pos, Point* jointPos,
                                                                  Point* postTurnPos,
                                                                  Direction prevDir) {
	int turns[NUM_DIRS], nTurns;
	IPOINT3D nextPos;
	int newDir;
	Point* nextNode;

	// First, check if next node along current dir is empty
	nextNode = getNextNodePos(pos, prevDir);// node out of bounds or not empty
	if(node_struct[nextNode->x][nextNode->y][nextNode->z]) { return DIR_NONE; }

	// Ok, the next node is free - check the 4 possible turns from here
	nTurns = getBestPossibleTurns(&nextPos, dir, turns);
	if(nTurns == 0) return DIR_STRAIGHT;// nowhere to turn, but could go straight

	// randomnly choose one of the possible turns
	newDir = turns[iRand(nTurns)];

	// SS_ASSERT((newDir >= 0) && (newDir < NUM_DIRS),
	//           "NODE_ARRAY::ChooseNewTurnDirection : invalid newDir\n");

	// mark taken nodes

	nextNode = getNode(&nextPos);
	nextNode->MarkAsTaken();

	nextNode = getNextNode(&nextPos, newDir);
	nextNode->MarkAsTaken();

	return newDir;
}