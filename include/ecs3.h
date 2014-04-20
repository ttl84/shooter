#ifndef ecs3_H
#define ecs3_H
#include <stack>
#include <queue>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
namespace ecs3{
	// polymorphic component arrays
	class ComponentBase{
	private:
		ComponentBase() = delete;
	public:
		virtual ~ComponentBase(){}
		virtual void swapEle(unsigned i, unsigned j) = 0;
	};
	
	template<class T>
	struct ComponentList final: public ComponentBase, public std::vector<T>{
		ComponentList(unsigned size): std::vector<T>(size){}
		ComponentList(ComponentList<T> const & ) = delete;
		ComponentList<T> const & operator = (ComponentList<T> const &) = delete;
		void swapEle(unsigned i, unsigned j) override
		{
			std::swap(this->at(i), this->at(j));
		}
	};
}

namespace ecs3{
	class System{
	private:
		System() = delete;
	public:
		virtual ~System(){}
		virtual void run() = 0;
	};
}

namespace ecs3{
	template<class cpnT, class maskT>
	class BasicWorld{
	public:
		typedef cpnT cpn_t;
		typedef maskT mask_t;
		typedef std::function<void(BasicWorld<cpnT, maskT> &, unsigned) > EntityInitFunc;
	private:
		

		// components
		std::vector<mask_t> masks;
		std::unordered_map<cpn_t, std::unique_ptr<ComponentBase>> components;
		std::stack<unsigned, std::vector<unsigned>> freelist;
		unsigned const size;

		// entity creation
		std::queue<EntityInitFunc> creationQueue;

		// systems
		std::vector<std::unique_ptr<System> > systems;
	public:
		// construct a world with fixed number of available slots
		BasicWorld(unsigned Size) :masks(Size), size(Size){}

		// disable copying;
		BasicWorld(BasicWorld const &) = delete;
		BasicWorld const & operator =(BasicWorld const &) = delete;

		// map a component number to a new component list
		template<class T>
		void addComponent(cpn_t id)
		{
			components.emplace(id, new ComponentList<T>(size));
		}
		
		// get a ref to a component list
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
	
		void addSystem(System * sys)
		{
			systems.emplace_back(sys);
		}

		void runSystems()
		{
			for (auto & sys : systems)
				sys->run();
		}

		void scheduleCreation(EntityInitFunc func)
		{
			creationQueue.push(func);
		}

		void executeCreation()
		{
			while(not creationQueue.empty())
			{
				auto func = creationQueue.front();
				creationQueue.pop();
				unsigned i = claim();
				func(*this, i);
			}
		}
		unsigned getSize() const
		{
			return size;
		}
	
		mask_t getMask(unsigned i) const
		{
			return masks(i);
		}
		void setMask(unsigned i, mask_t m)
		{
			masks[i] = m;
		}
		
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
				unsigned next = masks.size() - 1;
				return next;
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
