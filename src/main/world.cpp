#include "world.h"

void thinkSystem(World &w)
{
	for(auto i : w.select(Component::THINK))
		w.think_function[i](i);
}

void moveShips(World & w, float dt)
{
	auto mask = w.ships.position.mask & w.ships.velocity.mask & w.ships.acceleration.mask;
	for(unsigned i = 0; i < ships.length; i++) {
		if(mask[i]) {
			w.ships.velocity[i] += w.ships.acceleration[i] * dt;
			w.ships.position[i] += w.ships.velocity[i] * dt;
		}
	}
}
void turnShips(World & w, float dt)
{
	for(unsigned i = 0; i < ships.length; i++) {
		w.ships.angular_speed[i] += w.ships.angular_acceleration[i] * dt;
		w.ships.direction[i] += w.ships.angular_speed[i];
	}
}

void collisionSystem(World & w)
{
	for(unsigned i = 0; i < ships.length; i++) {
		Circ icirc(w.ships.position[i], w.ships.radius[i]);
		for(unsigned j = i + 1; j < ships.length; j++) {
			Circ jcirc(w.ships.position[j], w.ships.radius[j]);
			if(Circ::intersect(icirc, jcirc)) {
				w.schedule([i, j](World & w) {
					w.ships.collision_function[i](j);
					w.ships.collision_function[j](i);
				});
			}
		}
	}
}

void timerSystem(World & w, float dt)
{
	for(auto i : e.select(Component::TIMER))
	{
		auto & timer = e.timer[i];

		timer.remaining -= dt;
		if(timer.remaining <= 0 and timer.action != nullptr)
			timer.action(i);
	}
}

void deathSystem(World &w)
{
	for(auto i : w.select(Component::LIFE))
	{
		auto & life = w.life[i];
		if(life.value <= 0 and life.deathAction != nullptr)
			life.deathAction(i);
	}
}

void drawSystem(World & w, SDL_Renderer * renderer, Rect const & camera)
{
	for(auto i : e.select(draw_mask))
	{
		SDL_Rect posRect;
		SDL_QueryTexture(e.image[i], nullptr, nullptr, &posRect.w, &posRect.h);
		posRect.x = int(e.position[i].x) - posRect.w / 2 - int(camera.x);
		posRect.y = int(e.position[i].y) - posRect.h / 2 - int(camera.y);
		SDL_RenderCopyEx(renderer, e.image[i], nullptr, &posRect, rad2deg(e.direction[i] + PI / 2.0), nullptr, SDL_FLIP_NONE);

	}
}
