#include "world.h"
#include "state.h"
#include <iostream>



void drawSystem(World & w, SDL_Renderer * renderer, Rect const & camera)
{
	auto mask = w.ships.position.mask & w.ships.texture.mask & w.ships.rect.mask & w.ships.direction.mask;
	for(unsigned i = 0; i < decltype(w.ships)::length; i++) {
		if(mask[i]) {
			SDL_Rect posRect = w.ships.rect.data[i];
			posRect.x = int(w.ships.position.data[i].x) - posRect.w / 2 - int(camera.x);
			posRect.y = int(w.ships.position.data[i].y) - posRect.h / 2 - int(camera.y);
			SDL_RenderCopyEx(renderer,
							 w.ships.texture.data[i],
							 nullptr,
							 &posRect,
							 rad2deg(w.ships.direction.data[i] + PI / 2.0),
							 nullptr,
							 SDL_FLIP_NONE);
		}
	}
}


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
