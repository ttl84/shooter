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
};

enum class Faction{
	PLAYER,
	PLAYER_BULLET,
	ENEMY,
	ENEMY_BULLET
};

struct Space{
	Vec2 position;
	float angle;
	float radius;
};
struct SpaceDerivative{
	Vec2 position_d1;
	Vec2 position_d2;
	float angle_d1;
	float angle_d2;
};

#endif
