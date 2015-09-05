#ifndef GAME_HPP
#define GAME_HPP
#include "math/Rect.h"
#include <random>
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
#endif // GAME_HPP

