#ifndef WORLD_H
#define WORLD_H
#include "ecs.hpp"



struct State;
// world keeps all the game objects
class World{

	Entities entities;
public:
	void update(State & s, double dt);
};

#endif
