#ifndef PI_H
#define PI_H
float constexpr PI = 3.1415926;
float constexpr rad2deg(float rad)
{
	return rad * 180.0 / PI;
}
#endif