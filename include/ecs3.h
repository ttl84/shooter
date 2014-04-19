#ifndef ecs3_H
#define ecs3_H
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
namespace ecs3{
	// polymorphic component arrays
	class ComponentBase{
	private:
		ComponentBase() = delete;
	public:
		virtual ~ComponentBase(){}
	};
	
	template<class T>
	struct ComponentList final: public ComponentBase, public std::vector<T>{
		ComponentList(unsigned size): std::vector<T>(size){}
		ComponentList(ComponentList<T> const & ) = delete;
		ComponentList<T> const & operator = (ComponentList<T> const &) = delete;
	};
}


namespace ecs3{
	template<class cpnT, class maskT>
	class World{
	public:
		typedef cpnT cpn_t;
		typedef maskT mask_t;
	private:
		std::vector<mask_t> masks;
		std::unordered_map<cpn_t, std::unique_ptr<ComponentBase>> components;
		std::stack<unsigned, std::vector<unsigned>> freelist;
		unsigned const size;
	public:
		// construct a world with fixed number of available slots
		World(unsigned Size) :masks(Size), size(Size)
		{
			for(unsigned i = size; i != 0; i--)
				freelist.push(i);
		}
		// disable copying;
		World(World const &) = delete;
		World const & operator =(World const &) = delete;

		// map a component number to a new component list
		template<class T>
		void addComponent(cpn_t id)
		{
			components.emplace(id, new ComponentList<T>(size));
		}
		
		// get a pointer to a component list
		template<class T>
		ComponentList<T> & getComponent(cpn_t id)
		{
			auto it = components.find(id);
			if(it == components.end())
			{
				std::cerr << __func__ << ": id is not mapped to a component list" << std::endl;
				abort();
			}
			return *static_cast<ComponentList<T>*>(it->second.get());
		}
	
		unsigned getSize() const;
	
		mask_t getMask(unsigned i) const;
		void setMask(unsigned i, mask_t m);
		
		// number of slots that can be claimed
		unsigned slotsLeft() const
		{
			return freelist.size();
		}
	
		// claim a slot to make an entity
		unsigned claim()
		{
			if(not freelist.empty())
			{
				unsigned next = freelist.top();
				freelist.pop();
				return next;
			}
			else
			{
				std::cerr << __func__ << ": no more slots to claim" << std::endl;
				abort();
			}
		}
		// make the slot claimable again
		void relinquish(unsigned i)
		{
			if(freelist.size() >= this->size)
			{
				std::cerr << __func__ << ": no more slots to relinquish" << std::endl;
				abort();
			}
			else if(i >= size)
			{
				std::cerr << __func__ << ": slot is out of range" << std::endl;
				abort();
			}
			else
			{
				masks[i] = 0;
				freelist.push(i);
			}
		}
		
		// iterator
	
		class iterator {
		private:
			std::vector<mask_t> const & masks;
			mask_t const mask;
			unsigned i;
			unsigned const size;
			
		public:
			iterator(std::vector<mask_t> & ms, mask_t m, unsigned start)
				: masks(ms), mask(m), i(start), size(masks.size())
			{
				while(i < size and (masks[i] & mask) != mask)
					i++;
			}
			iterator& operator ++()
			{
				if(i < size)
					i++;
				while(i < size and (masks[i] & mask) != mask)
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
			std::vector<mask_t> & masks;
			mask_t const mask;
			iterator begin()
			{
				return iterator(masks, mask, 0);
			}
			iterator end()
			{
				return iterator(masks, mask, masks.size());
			}
		};
		template<class... Ts>
		View select(Ts ... args)
		{
			return View{masks, combine(args...)};
		}
	};
}
#endif
