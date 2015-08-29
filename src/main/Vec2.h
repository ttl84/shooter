#ifndef Vec2_H
#define Vec2_H
#include <cmath>
struct Vec2{
	float x, y;
	constexpr Vec2(void): x{0}, y{0} {}
	constexpr Vec2(float X, float Y): x{X}, y{Y} {}
		
	// create unit vector from angle
	static Vec2 fromAngle(float rad);
	float angle(void)const;
	float norm(void)const;
};
Vec2 operator+(Vec2 const & a, Vec2 const & b);
void operator+=(Vec2 & a, Vec2 const & b);

Vec2 operator-(Vec2 const & a, Vec2 const & b);
void operator-=(Vec2 & a, Vec2 const & b);
Vec2 operator-(Vec2 const & a);

Vec2 operator*(Vec2 const & a, float k);
Vec2 operator*(float k, Vec2 const & a);
void operator *=(Vec2 & a, float k);

Vec2 operator/(Vec2 const & a, float k);
Vec2 operator/(float k, Vec2 const & a);
void operator /=(Vec2 & a, float k);
#endif
