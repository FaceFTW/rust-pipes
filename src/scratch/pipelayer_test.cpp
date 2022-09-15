#include "../include/pipe_layer.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

	Point* bounds = new Point(10, 10, 10);
	GlPipes::PipeLayer* pipelayer = new GlPipes::PipeLayer(bounds, 3);

	pipelayer->generatePipe(0);
	// pipelayer->generatePipe(1);
	// pipelayer->generatePipe(2);

	pipelayer->outputPipePath(0);
}
