#include "Entity.h"
#include "Circ.h"
#include "Rect.h"
#include "PI.h"
#include <vector>
namespace ecs{
void Entity::thinkSystem(void)
{
	for(auto i : select(Component::THINK))
		think_function[i](i);
}

void Entity::shootSystem(float dt)
{
	for(auto i : select(shooter_mask))
	{
		// guns have a wait_time to wait before they can fire again
		if(gun[i].wait_time > 0)
			gun[i].wait_time -= dt;
		
		// spawn bullet if gun is ready
		if(gun[i].fire and gun[i].wait_time <= 0)
		{
			gun[i].wait_time += gun[i].delay;
			gun[i].gun_function(i);
		}
	}
}

void Entity::accelSystem(float dt)
{
	for(auto i : select(accel_mask))
		velocity[i] += dt * acceleration[i];
}

void Entity::moveSystem(float dt)
{
	for(auto i : select(move_mask))
		position[i] += dt * velocity[i];
}

void Entity::collisionSystem(void)
{
	// first construct collision shape for entities that can collide
	struct Body{
		entity_t id;
		Circ shape;
	};
	std::vector<Body> players;
	std::vector<Body> enemies;
	for(auto i : select(collision_effect_mask))
	{

		Body obj;
		obj.id = i;
		obj.shape = Circ(position[i], size[i].w / 2.5);
		if(faction[i] == Faction::PLAYER)
			players.push_back(obj);
		else if(faction[i] == Faction::ENEMY)
			enemies.push_back(obj);
	}
	
	// test the two groups against each other
	for(Body & i : players)
	{
		for(Body & j : enemies)
		{
			if(Circ::intersect(i.shape, j.shape))
			{
				collision_effect[i.id](j.id);
				collision_effect[j.id](i.id);
			}
		}
	}
}

void Entity::timerSystem(float dt)
{
	for(auto i : select(Component::TIMER))
	{
		timer[i] -= dt;
		if(timer[i] <= 0 and timer_function[i] != nullptr)
			timer_function[i](i);
	}
}

void Entity::deathSystem(void)
{
	for(auto i : select(Component::HEALTH))
	{
		bool dead = health[i] <= 0;
		if(dead and death_function[i] != nullptr)
			death_function[i](i);
	}
}

void Entity::drawSystem(SDL_Renderer * renderer, Rect const & camera)
{
	for(auto i : select(draw_mask))
	{
		SDL_Rect posRect;
		SDL_QueryTexture(image[i], nullptr, nullptr, &posRect.w, &posRect.h);
		posRect.x = int(position[i].x) - posRect.w / 2 - int(camera.x);
		posRect.y = int(position[i].y) - posRect.h / 2 - int(camera.y);
		SDL_RenderCopyEx(renderer, image[i], nullptr, &posRect, rad2deg(direction[i] + PI / 2.0), nullptr, SDL_FLIP_NONE);

	}
}
}
