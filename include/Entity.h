#ifndef ENTITY_H
#define ENTITY_H
#include "components.h"
#include "masks.h"
#include <stack>
#include "Rect.h"

namespace ecs{
	typedef unsigned entity_t;
	
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
				if(i < size)
					i++;
				while(i < size and (entity.mask[i] & mask) != mask)
					i++;
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
