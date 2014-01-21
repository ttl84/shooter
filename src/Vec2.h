#ifndef Vec2_H
#define Vec2_H
#include <cmath>
struct Vec2{
	float x, y;
	Vec2(void): x(0), y(0) {}
	Vec2(float X, float Y): x(X), y(Y) {}
	
	Vec2(float rad)
	{
		x = cos(rad);
		y = sin(rad);
	}
	float angle(void)const
	{
		return atan2(y, x);
	}
	float norm(void)const
	{
		return sqrt(x * x + y * y);
	}
};
Vec2 const operator+(Vec2 const & a, Vec2 const & b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}
Vec2 const & operator+=(Vec2 & a, Vec2 const & b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
Vec2 const operator-(Vec2 const & a, Vec2 const & b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}
Vec2 const & operator-=(Vec2 & a, Vec2 const & b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
Vec2 const operator-(Vec2 const & a)
{
	return Vec2(-a.x, -a.y);
}
Vec2 const operator*(Vec2 const & a, float k)
{
	return Vec2(a.x * k, a.y * k);
}
Vec2 const operator*(float k, Vec2 const & a)
{
	return Vec2(a.x * k, a.y * k);
}
Vec2 const & operator *=(Vec2 & a, float k)
{
	a.x *= k;
	a.y *= k;
	return a;
}
Vec2 const operator/(Vec2 const & a, float k)
{
	return Vec2(a.x / k, a.y / k);
}
Vec2 const operator/(float k, Vec2 const & a)
{
	return Vec2(a.x / k, a.y / k);
}
Vec2 const & operator /=(Vec2 & a, float k)
{
	a.x /= k;
	a.y /= k;
	return a;
}
#endif
