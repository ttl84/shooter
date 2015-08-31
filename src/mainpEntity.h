#ifndef ENTITY_H
#define ENTITY_H
#include "components.h"
#include "masks.h"
#include <stack>
#include <queue>

namespace ecs{
	constexpr unsigned MAX_ENTITIES = 256;
	class Entity{
	private:
		unsigned myCount;
		std::stack< unsigned > myHoles;
	public:
		mask_t mask[MAX_ENTITIES];
		
		Vec2 position[MAX_ENTITIES];
		Vec2 velocity[MAX_ENTITIES];
		Vec2 acceleration[MAX_ENTITIES];
		
		float direction[MAX_ENTITIES];
		float angular_speed[MAX_ENTITIES];
		float angular_acceleration[MAX_ENTITIES];
		
		Size size[MAX_ENTITIES];
		SDL_Texture * image[MAX_ENTITIES];
		
		Life life[MAX_ENTITIES];

		
		Heat heat[MAX_ENTITIES];
		Faction faction[MAX_ENTITIES];
		unsigned target[MAX_ENTITIES];
		
		std::function<void(unsigned)> think_function[MAX_ENTITIES];
		std::function<void(unsigned)> collision_effect[MAX_ENTITIES];
		
		// constructors
		Entity(void);
		Entity(Entity const &) = delete;
		Entity const & operator=(Entity const &) = delete;

		void reset();
		
		unsigned claim();
		
		void remove(unsigned i);
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
