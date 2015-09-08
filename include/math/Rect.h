#ifndef Rect_H
#define Rect_H
#include "Vec2.h"
#include "Size.h"

struct Rect{
	float left, right, top, bottom;
	
	Rect()
	{

	}

	Rect(Vec2 origin, Size size)
	{
		left = origin.x;
		bottom = origin.y;
		right = left + size.w;
		top = bottom + size.h;
		if(left > right)
			std::swap(left, right);
		if(bottom > top)
			std::swap(bottom, top);
	}

	void operator+=(Vec2 v)
	{
		left += v.x;
		right += v.x;
		bottom += v.y;
		top += v.y;
	}
	Vec2 getCenter() const
	{
		return Vec2(left / 2 + right / 2,
					top / 2 + bottom / 2);
	}
	void setCenter(Vec2 newCenter)
	{
		*this += newCenter - getCenter();
	}
	Vec2 getOrigin() const
	{
		return Vec2(left, bottom);
	}
	void setOrigin(Vec2 newOrigin)
	{
		*this += newOrigin - getOrigin();
	}

	bool contains(Vec2 p)const
	{
		return left <= p.x &&
				right >= p.x &&
				top >= p.y &&
				bottom <= p.y;
	}

};

inline
bool disjointX(Rect const & a, Rect const & b)
{
	return a.right <= b.left ||
			a.left >= b.right;
}
inline
bool disjointY(Rect const & a, Rect const & b)
{
	return a.top <= b.bottom ||
			a.bottom >= b.top;
}
inline
bool disjoint(Rect const & a, Rect const & b)
{
	return disjointX(a, b) || disjointY(a, b);
}

inline
bool intersectX(Rect const & a, Rect const & b)
{
	return (a.left < b.left && a.right > b.left) ||
			(a.left < b.right && a.right > b.right);
}

inline
bool intersectY(Rect const & a, Rect const & b)
{
	return (a.bottom < b.bottom && a.top > b.bottom) ||
			(a.bottom < b.top && a.top > b.top);
}

inline
bool intersect(Rect const & a, Rect const & b)
{
	return intersectX(a, b) &&  intersectY(a, b);
}
inline
Rect boundingBlock(Rect const & r, float blockWidth, float blockHeight)
{
	Rect bound;
	bound.top = std::ceil(r.top / blockHeight) * blockHeight;
	bound.bottom = std::floor(r.bottom / blockHeight) * blockHeight;
	bound.left = std::floor(r.left / blockWidth) * blockWidth;
	bound.right = std::ceil(r.right / blockWidth) * blockWidth;
	return bound;
}



#endif
