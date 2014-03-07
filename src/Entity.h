#ifndef ENTITY_H
#define ENTITY_H
#include "components.h"
#include <stack>
#include "Rect.h"
unsigned constexpr BIT(unsigned n)
{
	return 1 << n;
}
namespace ecs{
	typedef uint16_t mask_t;
	typedef unsigned entity_t;
	
	enum flag{
		POSITION = BIT(0),
		VELOCITY = BIT(1),
		ACCELERATION = BIT(2),
		DIRECTION = BIT(3),
		SIZE = BIT(4),
		
		IMAGE = BIT(5),
		
		HEALTH = BIT(6),
		TIMER = BIT(7),
		COLLISION_EFFECT = BIT(8),
		
		THINK = BIT(9),
		
		FACTION = BIT(10),
		GUN = BIT(11),
		CAMERA_FOCUS = BIT(12),
		TARGET = BIT(13)
		
	};
	
	constexpr unsigned MAX_ENTITIES = 1000;
	struct Entity{
		mask_t mask[MAX_ENTITIES];
		Vec2 position[MAX_ENTITIES];
		Vec2 velocity[MAX_ENTITIES];
		Vec2 acceleration[MAX_ENTITIES];
		double direction[MAX_ENTITIES];
		Size size[MAX_ENTITIES];
		SDL_Texture * image[MAX_ENTITIES];
		
		int health[MAX_ENTITIES];
		float timer[MAX_ENTITIES];
		entity_t target[MAX_ENTITIES];
		
		Gun gun[MAX_ENTITIES];
		Faction faction[MAX_ENTITIES];
		
		std::function<void(entity_t)> think_function[MAX_ENTITIES];
		std::function<void(entity_t)> death_function[MAX_ENTITIES];
		std::function<void(entity_t)> timer_function[MAX_ENTITIES];
		std::function<void(entity_t)> collision_effect[MAX_ENTITIES];
		
		// important methods
		Entity(void);
		Entity(Entity const &) = delete;
		Entity const & operator=(Entity const &) = delete;
		entity_t claim(void);
		void remove(entity_t i);
		unsigned count(void) const;
		unsigned capacity(void) const;
		
		// systems
		void thinkSystem(void);
		void shootSystem(float dt);
		void accelSystem(float dt);
		void moveSystem(float dt);
		void collisionSystem(void);
		void timerSystem(float dt);
		void deathSystem(void);
		void drawSystem(SDL_Renderer * renderer, Rect const & camera);
		
	private:
		unsigned m_count;
		std::stack<entity_t> holes;
	};
	
	mask_t constexpr move_mask = POSITION | VELOCITY;
	mask_t constexpr accel_mask = VELOCITY | ACCELERATION;
	mask_t constexpr collision_effect_mask = POSITION | SIZE | COLLISION_EFFECT;
	mask_t constexpr shooter_mask = POSITION | DIRECTION | SIZE | GUN;
	mask_t constexpr player_mask = move_mask | ACCELERATION | shooter_mask | collision_effect_mask | IMAGE | HEALTH | THINK | CAMERA_FOCUS;
	mask_t constexpr enemy_mask = move_mask | shooter_mask | collision_effect_mask | IMAGE | HEALTH;
	mask_t constexpr bullet_mask = move_mask | collision_effect_mask | DIRECTION | IMAGE | TIMER | HEALTH;
	
}
#endif
