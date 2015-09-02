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
template<class T>
bool intersect(Circ a, Circ b)
{
	return (a.position - b.position).norm() < (a.r + b.r);
}
#endif
