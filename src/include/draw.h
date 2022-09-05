
#ifndef DRAW_H__
#define DRAW_H__

#include "./utils.h"

namespace Draw {
static void cylinder(Point pos, Point angle, float length, float radius);
static void sphere(Point pos, float radius);
static void drawQuadStrip();

}// namespace Draw

#endif