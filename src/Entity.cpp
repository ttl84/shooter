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

namespace ecs2{
	enum class Component{
		POSITION,
		VELOCITY,
		ACCELERATION,
		GUN
	};
	template<Component i, class val_t>
	struct Decl : public mapping::Mapping<Component, val_t, i>{};
		

	template<Component i, class T>
	struct getType;
	
	template<Component i, class val_t, class...Ts>
	struct getType<i, list::List<Decl<i, val_t>, Ts...>> {
		typedef val_t type;
	};
	template<Component i, class T, class ... Ts>
	struct getType<i, list::List<T, Ts...>> : getType<i, list::List<Ts...>>{};
	

	
	template<Component i, class...Ts>
	auto ref(list::List<Ts...> & list) -> typename getType<i, list::List<Ts...>>::type&
	{
		typedef typename getType<i, list::List<Ts...>>::type val_t;
		typedef Decl<i, val_t> decl_t;
		return list::find< decl_t >(list).val;
	}
	
	typedef list::List<
		Decl<Component::POSITION, Vec2>,
		Decl<Component::VELOCITY, Vec2>,
		Decl<Component::ACCELERATION, Vec2>
	> World;
	static inline void hello()
	{
		World world;
		getType<Component::POSITION, World>::type vec;
		vec.x += 1;
		
		list::find< Decl<Component::POSITION, Vec2> >(world);
		ref<Component::POSITION>(world) = Vec2(1.2, 3.4);
		ref<Component::VELOCITY>(world) = Vec2(1.2, 3.4);
		ref<Component::ACCELERATION>(world) = Vec2(1.2, 3.4);
		
		// check if there are overhead
		static_assert(sizeof(World) == sizeof(Decl<Component::POSITION, Vec2>) * 3, "not same size");
	}
}