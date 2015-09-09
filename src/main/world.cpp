#include "world.h"
#include "state.h"
#include <iostream>



/*
// does what the ship wants
void shipControl(World & w)
{
	if(brain.accel) {
		Vec2 accel(0, -stats->.accel);
		accel.rotate(e.direction[i]);
		e.accel[i] = accel;
	} else {
		e.accel[i] = Vec2(0,0);
	}

	e.accel_ang[i] = 0;
	if(brain.turn_right)
		e.accel_ang[i] += stats->accel_ang;
	if(brain.turn_left)
		e.accel_ang[i] -= stats->accel_ang;



}
*/
void World::update(State &s, double dt)
{
	if(dead) {
		if(keyPress.any)
			reset();
	}
	think(ships);
	shootSystem(entities, dt);

	physicsSystem.update(dt);

	collisionSystem(entities);
	timerSystem(entities, dt);
	deathSystem(entities);

	update_camera(*this);
	update_bounds(*this);
	trap_player(*this);

	spawn_enemies(*this);
	despawn_entities(*this);

	updateStars();

	execute();
}
