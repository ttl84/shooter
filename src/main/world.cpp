#include "world.h"

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
		w.ships.direction.data[i] += w.ships.angular_speed.data[i];
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

void World::update(double dt)
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
