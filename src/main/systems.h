#ifndef systems_h
#define systems_h
#include "Entity.h"
#include "Rect.h"
#include "SDL2/SDL.h"
void thinkSystem(ecs::Entity & e);
void accelSystem(ecs::Entity & e, float dt);

void moveSystem(ecs::Entity & e, float dt);
void collisionSystem(ecs::Entity & e);
void timerSystem(ecs::Entity & e, float dt);
void deathSystem(ecs::Entity & e);

void drawSystem(ecs::Entity & e, SDL_Renderer * renderer, Rect const & camera);

#endif
