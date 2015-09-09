#ifndef RENDER_HPP
#define RENDER_HPP
#include "ecs.hpp"
#include "SDL2/SDL.h"
struct Texture {
	SDL_Texture * texture;
	SDL_Rect rect;
};

// Render system depends on the positions and orientations from
// the Physics system.
class PhysicsSystem;

class RenderSystem : ISystem {
	typedef typename IPackedArray::Slot Slot;
	typedef typename IPackedArray::ID ID;

	Entities & entities;
	PhysicsSystem * phys;
	PackedVector<Texture> data;
public:
	static constexpr char * name = "render";

	RenderSystem(Entities & ent);
	void update(double dt);

	char const * getName() const
	{
		return name;
	}

	IPackedArray& getComponents()
	{
		return data;
	}
};

#endif // RENDER_HPP

