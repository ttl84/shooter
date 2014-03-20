#include <vector>
#include <deque>
#include <map>
#include <string>
#include <functional>

#include "SDL2/SDL.h"

#include "GameState.h"

#include "Entity.h"
#include "components.h"

#include "Rect.h"
#include "Circ.h"
#include "PI.h"

#include "images.h"
#include "input.h"

#include "debug.h"





namespace player_speed{
	float fast = 200;
	float slow = 50;
	float normal = slow + (fast - slow) / 2.0;
}
Gun player_gun(ecs::Entity & entities, GameState & state)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 550.0;
	g.wait_time = 0;
	g.gun_function = [&entities, &state](ecs::entity_t self) -> void
	{
		
		Gun const & gun = entities.gun[self];
		
		ecs::entity_t bullet = entities.claim();
		entities.mask[bullet] = ecs::bullet_mask;
		
		entities.image[bullet] = images::bullets[0].getTexture(state.getRenderer());
		
		entities.position[bullet] = entities.position[self];
		entities.velocity[bullet] = entities.velocity[self] + gun.bullet_speed * 
			Vec2::fromAngle(entities.direction[self] + state.randFloat(-0.01, 0.01));
		entities.direction[bullet] = entities.direction[self];
		
		entities.collision_effect[bullet] = [&entities](ecs::entity_t i){
			entities.health[i] -= 1;
		};
		entities.faction[bullet] = Faction::PLAYER;
		
		entities.health[bullet] = 2;
		
		// visual effect for bullet if it hits something
		entities.death_function[bullet] = [&entities, &state](ecs::entity_t bullet) -> void{
			entities.timer[bullet] = 0.5;
			entities.image[bullet] = images::explosions[0].getTexture(state.getRenderer());
			entities.mask[bullet] = ecs::TIMER | ecs::IMAGE | ecs::POSITION;
			
		};
		
		entities.timer[bullet] = 2;
		entities.timer_function[bullet] = [&entities](ecs::entity_t self){entities.remove(self);};
	};
	return g;
}
Gun basic_gun(ecs::Entity & entities, GameState & state)
{
	Gun g;
	g.delay = state.randFloat(0.2, 1.0);
	g.bullet_speed = state.randFloat(50.0, 150.0);
	g.wait_time = 0;
	g.gun_function = [&entities, &state](ecs::entity_t i)
	{
		ecs::entity_t bullet = entities.claim();
		entities.mask[bullet] = ecs::bullet_mask;
		
		entities.position[bullet] = entities.position[i];
		
		ecs::entity_t target = entities.target[i];
		Vec2 direction = entities.position[target] - entities.position[i];
		if(direction.norm() > 0)
			direction /= direction.norm();
		entities.velocity[bullet] = entities.velocity[i] + entities.gun[i].bullet_speed * direction;
		entities.image[bullet] = images::bullets[1].getTexture(state.getRenderer());
		entities.timer[bullet] = 4.0;
		entities.timer_function[bullet] = [&entities](ecs::entity_t i){entities.health[i] = 0;};
		
		entities.health[bullet] = 1;
		entities.death_function[bullet] = [&entities](ecs::entity_t i){entities.remove(i);};
		
		entities.collision_effect[bullet] = [&entities](ecs::entity_t i){
			entities.health[i] -= 1;
		};
		entities.faction[bullet] = entities.faction[i];
		
	};
	g.fire = true;
	return g;
}

ecs::entity_t get_player(ecs::Entity & entities)
{
	ecs::entity_t i;
	for(i = 0; i < entities.count(); i++)
		if(entities.mask[i] & ecs::CAMERA_FOCUS)
			break;
	return i;
}
void keyboard_control(ecs::Entity & entities, ecs::entity_t i)
{	
	float speed = entities.velocity[i].norm();
	
	// makes sure player  doesn't move backwards
	if(fabs(entities.velocity[i].y) > player_speed::fast)
		entities.velocity[i].y = player_speed::fast * -1;
	if(fabs(entities.velocity[i].y) < player_speed::slow)
		entities.velocity[i].y = player_speed::slow * -1;
	
	
	if(control::faster)
	{
		if(speed < player_speed::fast)
			entities.acceleration[i].y = -30;
		else
			entities.acceleration[i].y = 0;
	}
	else if(control::slower)
	{
		if(speed > player_speed::slow)
			entities.acceleration[i].y = 30;
		else
			entities.acceleration[i].y = 0;
	}
	else
	{
		entities.acceleration[i].y = 0;
	}
	
	if(control::left)
	{
		entities.acceleration[i].x = (entities.velocity[i].x >= 0 ? -200 : -100);
	}
	else if(control::right)
	{
		entities.acceleration[i].x = (entities.velocity[i].x <= 0 ? 200 : 100);
	}
	else
	{
		entities.acceleration[i].x = -1 * entities.velocity[i].x;
	}
	entities.direction[i] = entities.velocity[i].angle();
	if(control::fire)
	{
		entities.gun[i].fire = true;
	}
	else
	{
		entities.gun[i].fire = false;
	}
	
}

void spawn_player(ecs::Entity & entities, GameState & state)
{
	ecs::entity_t player = entities.claim();
	
	entities.mask[player] = ecs::player_mask;
	
	entities.image[player] = images::players[0].getTexture(state.getRenderer());
	
	{
		int w, h;
		SDL_QueryTexture(entities.image[player], nullptr, nullptr, &w, &h);
		entities.size[player] = Size(w, h);
	}
	entities.position[player] = state.camera.getCenter();
	
	entities.velocity[player] = player_speed::normal * Vec2(0, -1);
	entities.acceleration[player] = Vec2(0, 0);
	entities.direction[player] = Vec2(0, -1).angle();
	
	
	entities.gun[player] = player_gun(entities, state);
	
	entities.health[player] = 3;
	
	entities.collision_effect[player] = [&entities](ecs::entity_t i){
		entities.health[i] -= 10;
	};
	
	entities.faction[player] = Faction::PLAYER;
	
	entities.think_function[player] = [&entities](ecs::entity_t self){keyboard_control(entities, self);};
	entities.death_function[player] = [&entities](ecs::entity_t self){entities.remove(self);};
}
ecs::entity_t spawn_enemy(ecs::Entity & entities, GameState & state)
{
	ecs::entity_t enemy = entities.claim();
	
	entities.mask[enemy] = ecs::enemy_mask;
	
	entities.image[enemy] = images::enemies[0].getTexture(state.getRenderer());
	
	{
		int w, h;
		SDL_QueryTexture(entities.image[enemy], nullptr, nullptr, &w, &h);
		entities.size[enemy] = Size(w, h);
	}
	{
		float randomX = state.randFloat(state.camera.getLeft(), state.camera.getRight());
		entities.position[enemy] = Vec2(randomX, state.bounds.getTop());
	}
	{
		float randomSpeed = state.randFloat(25.0, 70.0);
		entities.velocity[enemy] = Vec2(0, randomSpeed);
	}
	entities.direction[enemy] = Vec2(0, 1).angle();
	
	
	entities.gun[enemy] = basic_gun(entities, state);
	
	entities.health[enemy] = 3;
	
	entities.collision_effect[enemy] = [&entities](ecs::entity_t i){
		entities.health[i] -= 2;
	};
	
	entities.faction[enemy] = Faction::ENEMY;
		
	entities.target[enemy] = get_player(entities);
	
	entities.death_function[enemy] = [&entities, &state](ecs::entity_t enemy) -> void
	{
		entities.health[enemy] = 1;
		entities.timer[enemy] = 0.5;
		entities.image[enemy] = images::explosions[2].getTexture(state.getRenderer());
		entities.mask[enemy] = ecs::IMAGE | ecs::TIMER | ecs::POSITION;
		entities.mask[enemy] |= ecs::TIMER;
		entities.timer_function[enemy] = [&entities](ecs::entity_t enemy){entities.remove(enemy);};
	};
	return enemy;
}

ecs::entity_t spawn_enemy2(ecs::Entity & entities, GameState & state)
{
	ecs::entity_t enemy = spawn_enemy(entities, state);
	entities.position[enemy].y = state.bounds.getBottom();
	entities.velocity[enemy] = Vec2(0, -player_speed::fast);
	entities.direction[enemy] = Vec2(0, -1).angle();
	
	entities.image[enemy] = images::enemies[0].getTexture(state.getRenderer());
	
	entities.mask[enemy] |= ecs::THINK | ecs::ACCELERATION | ecs::TIMER;
	entities.think_function[enemy] = [&entities](ecs::entity_t i){
		ecs::entity_t target = entities.target[i];
		entities.velocity[i].y = entities.velocity[target].y + (-50);
		entities.think_function[i] = [](ecs::entity_t i){
			
		};
	};
	entities.timer[enemy] = 10.0;
	entities.timer_function[enemy] = [&entities, &state](ecs::entity_t i){
		entities.think_function[i] = [&entities, &state](ecs::entity_t i){
			entities.acceleration[i] = Vec2(state.randFloat(-50, 50), -50);
		};
	};
	return enemy;
}

void spawn_enemies(ecs::Entity & entities, GameState & state)
{
	float roll = state.randRoll();
	if(roll < 0.0001)
		spawn_enemy2(entities, state);
	else if(roll < 0.0002)
		spawn_enemy(entities, state);
}



void update_camera(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::CAMERA_FOCUS) == ecs::CAMERA_FOCUS)
		{
			float center_y = entities.position[i].y;
			state.updateCurrentY(center_y);
			float adjustment = entities.velocity[i].y + player_speed::normal;
			float multiplier = state.windowHeight * 0.95 / (player_speed::fast - player_speed::slow);
			state.camera.setCenterY(floor(center_y - adjustment * multiplier));
			return;
		}
	}
			
}
void update_bounds(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::CAMERA_FOCUS) == ecs::CAMERA_FOCUS)
		{
			Vec2 center = entities.position[i];
			state.updateBounds(center);
			return;
		}
	}
			
}
void despawn_entities(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(entities.mask[i] & ecs::POSITION)
		{
			if(not state.bounds.contains(entities.position[i]))
			{
				entities.remove(i);
			}
		}
	}
}


std::deque<Vec2> stars;

void spawn_star(GameState & state)
{
	static std::mt19937 genRoll(0);
	static std::mt19937 genX(1);
	float roll = std::generate_canonical<float, 20>(genRoll);
	if(roll < 0.01)
	{
		auto distr = std::uniform_real_distribution<float>(state.camera.getLeft(), state.camera.getRight());
		float randomX = distr(genX);
		stars.emplace_back(randomX, state.bounds.getTop());
	}
}
void despawn_star(GameState & state)
{
	while((not stars.empty()) and (stars.front().y > state.camera.getBottom()))
	{
		stars.pop_front();
	}
}
void update(ecs::Entity & entities, GameState & state, float const dt)
{
	entities.thinkSystem();
	entities.shootSystem(dt);
	
	entities.accelSystem(dt);
	entities.moveSystem(dt);
	
	entities.collisionSystem();
	entities.timerSystem(dt);
	entities.deathSystem();
	
	update_camera(entities, state);
	update_bounds(entities, state);
	
	spawn_enemies(entities, state);
	despawn_entities(entities, state);
	spawn_star(state);
	despawn_star(state);
	
}
void draw_stars(GameState & state)
{
	SDL_Texture * star_tex = images::stars[0].getTexture(state.getRenderer());
	for(Vec2 & star : stars)
	{
		SDL_Rect position;
		position.x = star.x - state.camera.x;
		position.y = star.y - state.camera.y;
		SDL_QueryTexture(star_tex, nullptr, nullptr, &position.w, &position.h);
		SDL_RenderCopy(state.getRenderer(), star_tex, nullptr, &position);
	}
}

void draw(ecs::Entity & entities, GameState & state)
{
	SDL_SetRenderDrawColor(state.getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(state.getRenderer());
	
	draw_stars(state);
	entities.drawSystem(state.getRenderer(), state.camera);
	
	SDL_RenderPresent(state.getRenderer());
}
#include "FileReader.h"
#include <sstream>
int main(int argc, char ** argv)
{
	{
		std::stringstream iss("1 -1 1.1 -1.1 1e10 -1e10 1.1e10 -1.1e10 1.1e-10");
		FileReader fr(iss);
	}
	float constexpr TIME_UNIT = 1.0 / 1000.0;
	float constexpr MAX_DT = 1.0 / 30.0;
	static ecs::Entity entities;
	static GameState state("shooter game", 480, 480);
	state.init();
	
	spawn_player(entities, state);
	
	Uint32 frame_begin = 0, frame_end = 0;
	float dt = TIME_UNIT;
	while(not control::quit)
	{
		frame_begin = frame_end;
		
		handle_event();
		if(dt > MAX_DT)
			dt = MAX_DT;
		while(dt > 0)
		{
			update(entities, state, TIME_UNIT);
			dt -= TIME_UNIT;
		}
		draw(entities, state);
		
		frame_end = SDL_GetTicks();
		dt += float(frame_end - frame_begin) / 1000.0;
	}
	state.cleanup();
	return 0;
}
