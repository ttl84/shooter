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
	float distanceTraveled;
	float timeElapsed;
	unsigned kills; // # of ships destroyed
	unsigned grazed; // # of bullets or ships grazed
	unsigned bulletsDeflected; // # number of enemy bullets hit by your own

	uint64_t score;

	bool dead; // once this is set, game waits until player wants to reset
	bool quit; // once this is set, game should stop
	
	// random generators
	typedef std::mt19937 PRG;
	PRG starPRG;
	PRG enemySpawnPRG;

	// An area that surrounds the player.
	// This area is used for spawning and despawning.
	Rect bounds;

};



// All state of the program
struct State{
	Video video;
	Audio audio;
	Keyboard keyboard;
	Window * window;
	Game game;
	World world;
	void update(float dt);
	void draw();
};

#endif
