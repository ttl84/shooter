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

void shootSystem(Entity & e, float dt)
{
	for(auto i : e.select(shooter_mask))
	{
		// guns have a wait_time to wait before they can fire again
		if(e.gun[i].wait_time > 0)
			e.gun[i].wait_time -= dt;
		
		// spawn bullet if gun is ready
		if(e.gun[i].fire and e.gun[i].wait_time <= 0)
		{
			e.gun[i].wait_time += e.gun[i].delay;
			e.gun[i].gun_function(i);
		}
	}
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
		e.timer[i] -= dt;
		if(e.timer[i] <= 0 and e.timer_function[i] != nullptr)
			e.timer_function[i](i);
	}
}

void deathSystem(Entity & e)
{
	for(auto i : e.select(Component::LIFE))
	{
		bool dead = e.life[i].health <= 0;
		if(dead and e.life[i].death_function != nullptr)
			e.life[i].death_function(i);
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
