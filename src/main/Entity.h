#ifndef WORLD_H
#define WORLD_H
#include "components.h"
#include <array>
#include <bitset>

template<unsigned n, class T>
class Array {
	std::bitset<n> mask;
	std::array<T, n> data;
};
template<unsigned n>
struct Ships{
	Array<n, Vec2> position;
	Array<n, Vec2> velocity;
	Array<n, Vec2> acceleration;

	Array<n, float> direction;
	Array<n, float> angular_speed;
	Array<n, float> angular_momentum;

	Array<n, Circ> circ;
	Array<n, SDL_Rect> rect;
	Array<n, SDL_Texture*> texture;

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

}
#endif
