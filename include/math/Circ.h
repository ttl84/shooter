#ifndef Circ_H
#define Circ_H
#include <cmath>
#include "pie.h"
#include "Vec2.h"
struct Circ{
	Vec2 position;
	float r;
	Circ(void) : position(0, 0), r(0) {}
	Circ(float X, float Y, float R) : position(X, Y), r(fabs(R)) {}
	Circ(Vec2 v, float R) : position(v), r(fabs(R)) {}

};
inline
bool intersect(Circ const & a, Circ const & b)
{
	Vec2 diff = a.position - b.position;
	float distanceSquared = dotProduct(diff, diff);
	float distance2 = a.r + b.r;
	float distance2Squared = distance2 * distance2;
	return distanceSquared < distance2Squared;
}
#endif
