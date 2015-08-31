#include "systems.h"
#include "Entity.h"
#include "components.h"
#include "Circ.h"
#include "Rect.h"
#include "PI.h"
#include <vector>
using namespace ecs;
void thinkSystem(Entity & e)
{
	for(auto i : e.select(Component::THINK))
		e.think_function[i](i);
}

void accelSystem(Entity & e, float dt)
{
	for(auto i : e.select(accel_mask))
		e.velocity[i] += dt * e.acceleration[i];
}

void moveSystem(Entity & e, float dt)
{
	for(auto i : e.select(move_mask))
		e.position[i] += dt * e.velocity[i];
}

void collisionSystem(Entity & e)
{
	// first construct collision shape for entities that can collide
	struct Body{
		unsigned id;
		Circ shape;
	};
	std::vector<Body> players;
	std::vector<Body> enemies;
	for(auto i : e.select(collision_effect_mask))
	{

		Body obj;
		obj.id = i;
		obj.shape = Circ(e.position[i], e.size[i].w / 2.5);
		if(e.faction[i] == Faction::PLAYER)
			players.push_back(obj);
		else if(e.faction[i] == Faction::ENEMY)
			enemies.push_back(obj);
	}
	
	// test the two groups against each other
	for(Body & i : players)
	{
		for(Body & j : enemies)
		{
			if(Circ::intersect(i.shape, j.shape))
			{
				e.collision_effect[i.id](j.id);
				e.collision_effect[j.id](i.id);
			}
		}
	}
}

void timerSystem(Entity & e, float dt)
{
	for(auto i : e.select(Component::TIMER))
	{
		auto & timer = e.timer[i];

		timer.remaining -= dt;
		if(timer.remaining <= 0 and timer.action != nullptr)
			timer.action(i);
	}
}

void deathSystem(Entity & e)
{
	for(auto i : e.select(Component::LIFE))
	{
		auto & life = e.life[i];
		if(life.value <= 0 and life.deathAction != nullptr)
			life.deathAction(i);
	}
}

void drawSystem(Entity & e, SDL_Renderer * renderer, Rect const & camera)
{
	for(auto i : e.select(draw_mask))
	{
		SDL_Rect posRect;
		SDL_QueryTexture(e.image[i], nullptr, nullptr, &posRect.w, &posRect.h);
		posRect.x = int(e.position[i].x) - posRect.w / 2 - int(camera.x);
		posRect.y = int(e.position[i].y) - posRect.h / 2 - int(camera.y);
		SDL_RenderCopyEx(renderer, e.image[i], nullptr, &posRect, rad2deg(e.direction[i] + PI / 2.0), nullptr, SDL_FLIP_NONE);

	}
}
