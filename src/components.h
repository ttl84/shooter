#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "Vec2.h"
#include "Size.h"
#include <functional>
#include "SDL2/SDL.h"
struct Gun{
	float delay; // time to wait after every fire
	float bullet_speed;
	
	float wait_time; // time left before the next fire
	bool fire;
	std::function<void(unsigned)> gun_function;
	Gun(void) : delay(0), bullet_speed(0), wait_time(0), fire(false), gun_function(nullptr) {}
};


enum class Faction{
	PLAYER,
	ENEMY
};
#endif
