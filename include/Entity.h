#ifndef ENTITY_H
#define ENTITY_H
#include "components.h"
#include <stack>
#include "Rect.h"

namespace ecs{
	typedef uint16_t mask_t;
	typedef unsigned entity_t;
	enum class Component{
		POSITION,
		VELOCITY,
		ACCELERATION,
		DIRECTION,
		SIZE,
		
		IMAGE,
		
		HEALTH,
		TIMER,
		COLLISION_EFFECT,
		
		THINK,
		
		FACTION,
		GUN,
		CAMERA_FOCUS,
		TARGET
		
	};
	
	// combine components and masks into one mask
	constexpr mask_t combine()
	{
		return 0;
	}
	template<class ... Ts>
	constexpr mask_t combine(mask_t head, Ts... tail)
	{
		return head | combine(tail...);
	}
	template<class ... Ts>
	constexpr mask_t combine(Component head, Ts... tail)
	{
		return ((mask_t)(1 << (unsigned) head)) | combine(tail...);
	}
	
	
	// check if a mask nothing (always true)
	template<class ... Ts>
	bool accepts(mask_t mask)
	{
		return true;
	}
	// checks if a mask satifies constant mask
	template<mask_t sub>
	bool accepts(mask_t mask)
	{
		return (mask & sub) == sub;
	}
	// checks if a mask satifies components
	template<Component... components>
	bool accepts(mask_t mask)
	{
		return accepts< combine(components...) >(mask);
	}

	mask_t constexpr move_mask = combine(
		Component::POSITION,
		Component::VELOCITY);
	
	mask_t constexpr accel_mask = combine(
		Component::VELOCITY,
		Component::ACCELERATION);
	
	mask_t constexpr collision_effect_mask = combine(
		Component::POSITION,
		Component::SIZE,
		Component::COLLISION_EFFECT);
	
	mask_t constexpr draw_mask = combine(
		Component::POSITION,
		Component::IMAGE);
	
	mask_t constexpr shooter_mask = combine(
		Component::POSITION,
		Component::DIRECTION,
		Component::SIZE,
		Component::GUN);
	
	mask_t constexpr player_mask = combine(
		move_mask,
		accel_mask,
		shooter_mask, 
		collision_effect_mask,
		Component::IMAGE,
		Component::HEALTH,
		Component::THINK,
		Component::CAMERA_FOCUS);
	
	mask_t constexpr enemy_mask = combine(
		move_mask,
		shooter_mask,
		collision_effect_mask,
		Component::IMAGE,
		Component::HEALTH);
	
	mask_t constexpr bullet_mask = combine(
		move_mask, 
		collision_effect_mask, 
		Component::DIRECTION,
		Component::IMAGE, 
		Component::TIMER, 
		Component::HEALTH);
	
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
		
		
		// iterator
		
		class iterator {
		private:
			Entity & entity;
			mask_t const mask;
			unsigned i;
			unsigned const size;
			
		public:
			iterator(Entity & e, mask_t m, unsigned start)
				: entity(e), mask(m), i(start), size(e.count())
			{
				while(i < size and (entity.mask[i] & mask) != mask)
					i++;
			}
			iterator& operator ++()
			{
				while(i < size)
				{
					i++;
					if(i == size or (entity.mask[i] & mask) == mask)
						break;
				}
				return *this;
			}
			unsigned operator * ()
			{
				return i;
			}
			bool operator!=(iterator const & other)
			{
				return i != other.i;
			}
		};
		struct View{
			Entity & entity;
			mask_t const mask;
			iterator begin()
			{
				return iterator(entity, mask, 0);
			}
			iterator end()
			{
				return iterator(entity, mask, entity.count());
			}
		};
		template<class... Ts>
		View select(Ts ... args)
		{
			return View{*this, combine(args...)};
		}
		
	private:
		unsigned myCount;
		std::stack<entity_t> myHoles;
	};
	
	
}

#endif
