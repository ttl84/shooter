#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "math/Vec2.h"
#include "math/Size.h"
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
namespace ecs{	
	enum class Component{
		POSITION,
			DIRECTION,
			VELOCITY,
			ANGULAR_VELOCITY,
			ACCELERATION,
			ANGULAR_ACCEL,
			SIZE,
	
			IMAGE,
			
			LIFE,
			HEAT,

			COLLISION_EFFECT,
		
			THINK,
		
			FACTION,
		
			TARGET
		
			};
}
#endif
