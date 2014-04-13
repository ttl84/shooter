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

// maps a constant to a variable
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
// it's like std::tuple
namespace list{
	template <class...Ts> class List{};
		
	template <class T, class ... Ts>
	struct List<T, Ts...>{
		T head;
		List<Ts...> tail;
	};

	template<class T, class ... Ts>
	T& find(List<T, Ts...> & list)
	{
		return list.head;
	}
	
	template<class T, class T1, class ... Ts>
	T& find(List<T1, Ts...> & list)
	{
		static_assert(sizeof...(Ts) != 0, "reached end of List, can't find type\n");
		return find<T>(list.tail);
	}
}

namespace ecs2{
	enum class Component{
		POSITION,
		VELOCITY,
		ACCELERATION
	};
	template<Component i, class val_t>
	struct Decl : public mapping::Mapping<Component, val_t, i>{};
		
	template<Component i, class val_t, class...Ts>
	val_t& ref(list::List<Ts...> & list)
	{
		return list::find<Decl<i, val_t>>(list);
	}
	
	typedef list::List<
		Decl<Component::POSITION, std::vector<Vec2>>,
		Decl<Component::VELOCITY, Vec2>,
		Decl<Component::ACCELERATION, Vec2>
	> World;
	
}