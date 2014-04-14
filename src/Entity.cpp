#include "Entity.h"
#include <iostream>
namespace ecs{
Entity::Entity(void) : mask{0}, myCount{0}
{
	
}
entity_t Entity::claim(void)
{
	entity_t next;
	if(myHoles.empty())
	{
		if(myCount == MAX_ENTITIES)
		{
			std::cerr << "out of slots for new entity, aborting\n";
			abort();
		}
		else
			next = myCount++;
	}
	else
	{
		next = myHoles.top();
		myHoles.pop();
	}
	image[next] = nullptr;
	target[next] = 0;
	think_function[next] = [](entity_t self){std::cerr << "missing think function\n";};
	death_function[next] = [](entity_t self){std::cerr << "missing death function\n";};
	timer_function[next] = [](entity_t self){std::cerr << "missing timer function\n";};
	return next;
}
void Entity::remove(entity_t i)
{
	mask[i] = 0;
	myHoles.push(i);
}
unsigned Entity::count(void) const
{
	return myCount;
}
unsigned Entity::capacity(void) const
{
	return MAX_ENTITIES;
}
}

// maps a constant value to a variable
namespace mapping{
	template<class key_t, class val_t, key_t k>
	struct Mapping{
		static constexpr key_t key = k;
		val_t val;
	};
	template<class key_t, class val_t, key_t k>
	constexpr Mapping<key_t, val_t, k> makeMapping()
	{
		return Mapping<key_t, val_t, k>{};
	}
}
// it's like a std::tuple
namespace list{
	template <class...Ts>
	class List{
		static constexpr unsigned length = 0;
	};
	template <class T>
	struct List<T>{
		T head;
		static constexpr unsigned length = 1;
	};
	template <class T, class ... Ts>
	struct List<T, Ts...>{
		T head;
		List<Ts...> tail;
		static constexpr unsigned length = decltype(tail)::length + 1;
	};

	template<class T>
	T& find(List<T> & list)
	{
		return list.head;
	}
	template<class T, class U, class ... Ts>
	T& find(List<T, U, Ts...> & list)
	{
		return list.head;
	}
	
	template<class T, class U, class ... Ts>
	T& find(List<U, Ts...> & list)
	{
		return find<T>(list.tail);
	}
}

#include <vector>
namespace ecs2{
	enum class Component{
		POSITION,
		VELOCITY,
		ACCELERATION,
		GUN,
		LAST
	};
	
	template<Component i, class val_t>
	struct Decl : public mapping::Mapping<Component, val_t, i>{};
		

	template<Component i, class T>
	struct getDeclType;
	
	template<Component i, class val_t, class...Ts>
	struct getDeclType<i, list::List<Decl<i, val_t>, Ts...>> {
		typedef val_t ValueType;
	};
	template<Component i, class T, class ... Ts>
	struct getDeclType<i, list::List<T, Ts...>> : getDeclType<i, list::List<Ts...>>{};
	

	
	template<Component i, class...Ts>
	auto ref(list::List<Ts...> & list) -> typename getDeclType<i, list::List<Ts...>>::ValueType&
	{
		typedef typename getDeclType<i, list::List<Ts...>>::ValueType val_t;
		typedef Decl<i, val_t> decl_t;
		return list::find< decl_t >(list).val;
	}
	

	class World{
	private:
		unsigned size;
		std::stack<unsigned> holes;
	public:
		std::vector<uint16_t> masks;
		list::List<
			Decl<Component::POSITION, std::vector<Vec2> >,
			Decl<Component::VELOCITY, std::vector<Vec2>>,
			Decl<Component::ACCELERATION, std::vector<Vec2>>
		> cpn;
		unsigned claim()
		{
			unsigned next;
			if(holes.empty())
			{
				next = size++;
				
			}
			else
			{
				next = holes.top();
				holes.pop();
			}
			return next;
		}
	};
	static inline void test()
	{
		World world;
		
		ref<Component::POSITION>(world.cpn).push_back(Vec2(1.2, 3.4));
		ref<Component::VELOCITY>(world.cpn).push_back(Vec2(1.2, 3.4));
		ref<Component::ACCELERATION>(world.cpn).push_back(Vec2(1.2, 3.4));
	}
}