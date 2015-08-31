#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "math/Vec2.h"
#include "math/Size.h"
#include "math/Rect.h"
#include "math/Circ.h"
#include <functional>
#include "SDL2/SDL.h"
struct Heat{
	float max_heat;
	float cool;
};

struct Life{
	int value;
	std::function<void(unsigned)> deathAction;
};

enum class Faction{
	PLAYER,
	ENEMY
};
#endif
