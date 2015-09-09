#include "render.hpp"
#include "physics.hpp"
RenderSystem::RenderSystem(Entities &ent)
	:entities(ent), phys(static_cast<PhysicsSystem*>(entities.findSystem(PhysicsSystem::name)))
{
	if(phys == nullptr) {
		throw std::string("failed to get physics system ref from ") + name + " system";
	}
	bool good = ent.registerSystem(this);
	if(!good) {
		throw std::string("failed to register ") + name + " system";
	}
}

void RenderSystem::update(double dt)
{
	for(Slot slot{0}; slot.value < data.length(); slot.value++) {
		Texture & texture = data.itemAtSlot(slot);
		ID id = data.idAtSlot(slot);

	}
}

/*
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
*/
