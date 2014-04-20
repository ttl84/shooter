#ifndef ENTITY_H
#define ENTITY_H
#include "components.h"
#include "masks.h"
#include "Rect.h"
#include <stack>
#include <queue>

namespace ecs{
	typedef unsigned entity_t;
	
	constexpr unsigned MAX_ENTITIES = 256;
	class Entity{
	private:
		unsigned myCount;
		std::stack< unsigned > myHoles;
		std::queue< std::function<void(Entity&, unsigned)> > creationQueue;
	public:
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
		
		// constructors
		Entity(void);
		Entity(Entity const &) = delete;
		Entity const & operator=(Entity const &) = delete;

		void reset();
		
		// modifiers
		void scheduleCreationJob(std::function<void(Entity&, unsigned)> initFunc);
		void executeCreationJobs();
	private:
		entity_t claim();
	public:
		
		void remove(entity_t i);
		unsigned count() const;
		unsigned capacity() const;
		
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
		
	
	};
	
	
}
#endif
