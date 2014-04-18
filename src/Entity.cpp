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

namespace ecs3{
	World::World(unsigned Size) :masks(Size), size(Size)
	{
		for(unsigned i = size; i != 0; i--)
			freelist.push(i);
	}
	template<class T>
	void World::addComponent(Component cpn)
	{
		components.emplace(cpn, new ComponentList<T>(size));
	}
	template<class T>
	ComponentBase * World::getComponent(Component cpn)
	{
		auto it = components.find(cpn);
		if(it != components.end())
			return static_cast<T>(it->second.get());
		else
			return nullptr;
	}
	
	unsigned World::slotsLeft() const
	{
		return freelist.size();
	}
	unsigned World::claim()
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
	void World::relinquish(unsigned i)
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
}