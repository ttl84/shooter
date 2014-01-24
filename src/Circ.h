#ifndef Circ_H
#define Circ_H
#include <cmath>
#include "PI.h"
#include "Vec2.h"
struct Circ{
	Vec2 position;
	float r;
	Circ(void) : position(0, 0), r(0) {}
	Circ(float X, float Y, float R) : position(X, Y), r(fabs(R)) {}
	Circ(Vec2 v, float R) : position(v), r(fabs(R)) {}
	bool intersects(Circ const & c) const
	{
		return (position - c.position).norm() < (r + c.r);
	}
};
#endif