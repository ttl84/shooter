#include "world.h"
#include "state.h"
unsigned IComponents::counter = 0;
void thinkSystem(World &w)
{
	for(unsigned i = 0; i < decltype(w.ships)::length; i++) {
		if(w.ships.control_function.mask[i]) {
			w.ships.control_function.data[i](i);
		}
	}
}

void moveShips(World & w, float dt)
{
	auto mask = w.ships.position.mask & w.ships.velocity.mask & w.ships.acceleration.mask;
	for(unsigned i = 0; i < decltype(w.ships)::length; i++) {
		if(mask[i]) {
			w.ships.velocity.data[i] += w.ships.acceleration.data[i] * dt;
			w.ships.position.data[i] += w.ships.velocity.data[i] * dt;
		}
	}
}
void turnShips(World & w, float dt)
{
	for(unsigned i = 0; i < decltype(w.ships)::length; i++) {
		w.ships.angular_speed.data[i] += w.ships.angular_acceleration.data[i] * dt;
		w.ships.direction.data[i] += w.ships.angular_speed.data[i] * dt;
	}
}

void collisionSystem(World & w)
{
	for(unsigned i = 0; i < decltype(w.ships)::length; i++) {
		Circ icirc(w.ships.position.data[i], w.ships.radius.data[i]);
		for(unsigned j = i + 1; j < decltype(w.ships)::length; j++) {
			Circ jcirc(w.ships.position.data[j], w.ships.radius.data[j]);
			if(intersect(icirc, jcirc)) {
				w.schedule([i, j](World & w) {
					w.ships.collision_function.data[i](j);
					w.ships.collision_function.data[j](i);
				});
			}
		}
	}
}

void timerSystem(World & w, float dt)
{

}

void deathSystem(World &w)
{

}

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
// cap ship values to its physical limits
void ship_constraint(World& w)
{
	ShipStats const * stats = e.shipstats[i];

	float speed = e.velocity[i].norm();
	if(speed > stats->max_speed)
		e.velocity[i] *= (stats->max_speed / speed);
	else if(speed < stats.min_speed)
		e.velocity[i] *= (stats->min_speed / speed);

	float speed_ang = e.speed_ang[i];
	if(speed_ang > stats->max_speed_ang)
		e.speed_ang[i] = stats->max_speed_ang;
	else if(speed_ang < stats->min_speed_ang)
		e.speed_ang[i] = stats->min_speed_ang;
}
void World::update(State &s, double dt)
{
	if(dead) {
		if(keyPress.any)
			reset();
	}
	think(ships);
	shootSystem(entities, dt);

	accelSystem(entities, dt);
	moveSystem(entities, dt);

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
