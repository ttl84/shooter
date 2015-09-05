#ifndef GameState_H
#define GameState_H

#include "world.h"
#include "stats.h"
#include "io.hpp"
#include "game.hpp"

struct State{
	IO io;
	Game game;
	World world;
	void update(float dt);
	void draw();
};

#endif
