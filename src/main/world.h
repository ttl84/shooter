#ifndef WORLD_H
#define WORLD_H
#include <array>
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

template<unsigned n>
struct Ships{
	static constexpr unsigned length = n;
	Array<n, Vec2> acceleration;
	Array<n, Vec2> velocity;
	Array<n, Vec2> position;

	Array<n, float> angular_acceleration;
	Array<n, float> angular_speed;
	Array<n, float> direction;

	// circle for collusion checks
	Array<n, float> radius;

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

	void update(double dt);
};

#endif
