#include "Entity.h"
#include <iostream>
namespace ecs{
Entity::Entity(void) : mask{0}, m_count{0}
{
	
}
entity_t Entity::claim(void)
{
	entity_t next;
	if(holes.empty())
	{
		if(m_count == MAX_ENTITIES)
		{
			std::cerr << "out of slots for new entity, aborting\n";
			abort();
		}
		else
			next = m_count++;
	}
	else
	{
		next = holes.top();
		holes.pop();
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
	holes.push(i);
}
unsigned Entity::count(void) const
{
	return m_count;
}
unsigned Entity::capacity(void) const
{
	return MAX_ENTITIES;
}
}