/**
 * @file pipe.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief Implementation of the Pipe Class
 * @version 0.1
 * @date 2022-07-05
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include "../include/pipe.h"

using namespace GlPipes;

/******ALIAS TYPEDEFS******/
typedef std::vector<PipePart> PipePartVector;

Pipe::Pipe(Point* startPos) { pipeParts = new PipePartVector(); }

Pipe::~Pipe() { delete pipeParts; }

//TODO
void Pipe::addPipePart(PipePart* part) { pipeParts->push_back(*part); }