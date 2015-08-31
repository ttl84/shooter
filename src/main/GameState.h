#ifndef GameState_H
#define GameState_H

#include "math/Rect.h"
#include "math/Vec2.h"

#include "video/video.h"
#include "audio/audio.h"
#include "keyboard/keyboard.h"
#include "Entity.h"
#include "stats.h"

#include <deque>
#include <string>
#include <random>
#include <unordered_map>

// Game keeps all the data related to the rules of the game
struct Game{
	unsigned dt;
	float currentY;
	float previousY;
	float timeElapsed;
	uint64_t score;
	bool dead;
	
	std::mt19937 PRG, starPRG, enemySpawnPRG;
	Rect bounds;
};

// world keeps all the game objects
struct World{
	std::unordered_map<std::string, Stat> stats;
	std::deque<Vec2> stars;
	ecs::Entity entities;

	std::queue< std::function< void(ecs::Entity&) > > worldUpdateQueue;
	void schedule(std::function< void(ecs::Entity&) > func);
	void execute();
};

// All state of the program
class State{
	Game game;
	World world;
	Audio audio;
	Video video;
	Window * window;
	Keyboard keyboard;
};

#endif
