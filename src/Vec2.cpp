#include "Vec2.h"
#include <cmath>
Vec2 Vec2::fromAngle(float rad)
{
	return Vec2(cos(rad), sin(rad));
}

float Vec2::norm(void)const
{
	return sqrt(x * x + y * y);
}
float Vec2::angle(void)const
{
	return atan2(y, x);
}
Vec2 operator+(Vec2 const & a, Vec2 const & b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}

void operator+=(Vec2 & a, Vec2 const & b)
{
	a.x += b.x;
	a.y += b.y;
}

Vec2 operator-(Vec2 const & a, Vec2 const & b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}

void operator-=(Vec2 & a, Vec2 const & b)
{
	a.x -= b.x;
	a.y -= b.y;
}

Vec2 operator-(Vec2 const & a)
{
	return Vec2(-a.x, -a.y);
}
Vec2 operator*(Vec2 const & a, float k)
{
	return Vec2(a.x * k, a.y * k);
}

Vec2 operator*(float k, Vec2 const & a)
{
	return Vec2(a.x * k, a.y * k);
}

void operator *=(Vec2 & a, float k)
{
	a.x *= k;
	a.y *= k;
}

Vec2 operator/(Vec2 const & a, float k)
{
	return Vec2(a.x / k, a.y / k);
}

Vec2 operator/(float k, Vec2 const & a)
{
	return Vec2(k / a.x,  k / a.y);
}

void operator /=(Vec2 & a, float k)
{
	a.x /= k;
	a.y /= k;
}