#ifndef GameState_H
#define GameState_H

#include "math/Rect.h"
#include "math/Vec2.h"

#include "video/video.h"
#include "audio/audio.h"
#include "keyboard/keyboard.h"
#include "world.h"
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
