#ifndef Vec2_H
#define Vec2_H
#include <cmath>

struct Vec2{
	float x, y;
	constexpr Vec2(): x{0}, y{0} {}
	constexpr Vec2(float X, float Y): x{X}, y{Y} {}
		
	// create unit vector from angle
	static Vec2 fromAngle(float rad);
	float angle()const;
	float norm()const;
};

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
inline
float dotProduct(Vec2 const & a, Vec2 const & b)
{
	return a.x * b.x + a.y * b.y;
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
#endif
