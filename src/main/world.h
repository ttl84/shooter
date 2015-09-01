#ifndef WORLD_H
#define WORLD_H
#include <array>
#include <valarray>
#include <bitset>
#include <unordered_map>
#include <queue>

#include "components.h"
#include "stats.h"

template<unsigned n, class T>
struct Array {
	std::bitset<n> mask;
	std::array<T, n> data;
};

template<unsigned n, class T>
struct Valarray{
	std::bitset<n> mask;
	std::valarray<T> data;
	Valarray() :data(n){}
};

template<unsigned n>
struct Ships{
	// 2-vectors stored as a double length valarray
	// illustration: {x1, y1, x2, y2, x3, y3...}
	Valarray<2*n, float> acceleration;
	Valarray<2*n, float> velocity;
	Valarray<2*n, float> position;

	// 1 float for every ship to represent direction
	Valarray<n, float> angular_acceleration;
	Valarray<n, float> angular_speed;
	Valarray<n, float> direction;

	// circle for collusion checks
	Array<n, Circ> circ;

	// rectangle and texture for drawing
	Array<n, SDL_Rect> rect;
	Array<n, SDL_Texture*> texture;

	// ship resources
	Array<n, Life> life;
	Array<n, Heat> heat;

	Array<n, Faction> faction;
	Array<n, unsigned> target;

	Array<n, std::function<void(unsigned)> > control_function;
	Array<n, std::function<void(unsigned)> > collision_function;
};

template<unsigned n>
struct Stars {
	Array<n, Vec2> position;
	Array<n, SDL_Texture*> texture;
};

// world keeps all the game objects
struct World{
	std::unordered_map<std::string, Stat> stats;

	Stars<64> stars;
	Ships<256> ships;

	std::queue< std::function< void(World&) > > worldUpdateQueue;
	void schedule(std::function< void(World&) > func)
	{
		worldUpdateQueue.push(func);
	}
	void execute()
	{
		while(not worldUpdateQueue.empty()) {
			worldUpdateQueue.front()(*this);
			worldUpdateQueue.pop();
		}
	}
};

void thinkSystem(World & w);
void accelSystem(World & w, float dt);

void moveSystem(World & w, float dt);
void collisionSystem(World & w);
void timerSystem(World & w, float dt);
void deathSystem(World & w);

void drawSystem(ecs::Entity & e, SDL_Renderer * renderer, Rect const & camera);

#endif
