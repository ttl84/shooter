#ifndef masks_h
#define masks_h
#include "components.h"
namespace ecs{
	typedef uint16_t mask_t;
	// combine components and masks into one mask
	constexpr mask_t combine()
	{
		return 0;
	}
	template<class ... Ts>
	constexpr mask_t combine(mask_t head, Ts... tail)
	{
		return head | combine(tail...);
	}
	template<class ... Ts>
	constexpr mask_t combine(Component head, Ts... tail)
	{
		return ((mask_t)(1 << (unsigned) head)) | combine(tail...);
	}
	
	// checks if a mask satifies constant mask
	template<mask_t sub>
	bool accepts(mask_t mask)
	{
		return (mask & sub) == sub;
	}
	
	mask_t constexpr move_mask = combine(
		Component::POSITION,
		Component::VELOCITY);
	
	mask_t constexpr accel_mask = combine(
		Component::VELOCITY,
		Component::ACCELERATION);
	
	mask_t constexpr collision_effect_mask = combine(
		Component::POSITION,
		Component::SIZE,
		Component::COLLISION_EFFECT);
	
	mask_t constexpr draw_mask = combine(
		Component::POSITION,
		Component::IMAGE);
	
	mask_t constexpr shooter_mask = combine(
		Component::POSITION,
		Component::DIRECTION,
		Component::SIZE
		);
	
	mask_t constexpr player_mask = combine(
		move_mask,
		accel_mask,
		shooter_mask, 
		collision_effect_mask,
		Component::IMAGE,
		Component::LIFE,
		Component::THINK
		);
	
	mask_t constexpr enemy_mask = combine(
		move_mask,
		shooter_mask,
		collision_effect_mask,
		Component::IMAGE,
		Component::LIFE);
	
	mask_t constexpr bullet_mask = combine(
		move_mask, 
		collision_effect_mask, 
		Component::DIRECTION,
		Component::IMAGE,
		Component::LIFE);
}
#endif
