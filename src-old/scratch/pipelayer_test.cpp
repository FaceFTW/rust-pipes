#include "../include/pipe_layer.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <pcg_basic.h>

int main(void) {
	pcg32_srandom(time(nullptr), 100000u);

	DrawConfig* cfg = new DrawConfig{14, 16, 3, 16, 16, 5};

	Point bounds = {10, 10, 10};
	GlPipes::PipeLayer* pipelayer = new GlPipes::PipeLayer(bounds, 3);

	pipelayer->generatePipe(0, cfg);
	// pipelayer->generatePipe(1);
	// pipelayer->generatePipe(2);

	pipelayer->outputPipePath(0);
}