#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <deque>
#include <stack>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include <SDL2/SDL.h>

#include "Entity.h"
#include "components.h"
#include "Rect.h"
#include "Circ.h"
#include "PI.h"
#include "TextureStorage.h"
#include "CharImg.h"
#include "input.h"
CharImg const explosion_pixels[3] = {
{R"(
    a
   b  ac
  abcdcd
   acbc
    c
)"}, {R"(
     a ba
 bcadc  acb
cb b cdd
aca cbcb
  cdabc  cb
abcbddb
  bdcb  abcd
)"}, {R"(
   ccbca
  cabacbaba
    acba
 c abcaa
)"}};
CharImg const player_pixels{R"(
        *        
       ***       
       * *       
      ** **      
      ** **      
     **   **     
     ** * **     
     ** * **     
    *********    
     *******     
    * ***** *    
   *   ***   *   
  *  * *** *  *  
 ** * * * * * ** 
 ***   ***   *** 
  **         **  
   *         *   
)"};

CharImg const enemy_pixels{R"(
        a        
        a        
       b b       
      ba ab      
     bba abb     
    b aa aa b    
   b   a a   b   
      ca ac      
    ccccacccc    
   ccccc ccccc   
  cdd c a c ddc  
 ccdd a a a ddcc 
ccd    aaa    dcc
cd     aaa     dc
 d      a      d 
 d      a      d 
        a
)"};
CharImg const laser_pixels{R"(
*
*
*
* 
*
* 
*
* 
*
* 
*
* 
*
* 
*
* 
*
* 
*
* 
*
)"};

CharImg const bullet_pixels{R"(
  *  
  *  
*****
  *  
  *
)"};

CharImg const star_pixels("*");



float const dt_unit = 1.0 / 1000.0;
unsigned window_width = 360;
unsigned window_height = 360;

SDL_Window * screen = nullptr;
SDL_Renderer * renderer = nullptr;
Rect camera;



std::deque<Vec2> stars;

TextureStorage textures;



namespace player_speed{
	float fast = 200;
	float slow = 50;
	float normal = slow + (fast - slow) / 2.0;
}
Gun player_gun(ecs::Entity & entities)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 550;
	g.wait_time = 0;
	g.gun_function = [&entities](ecs::entity_t self) -> void
	{
		
		Gun const & gun = entities.gun[self];
		
		ecs::entity_t bullet = entities.claim();
		entities.mask[bullet] = ecs::bullet_mask;
		
		entities.image[bullet] = textures.load(laser_pixels, {{'*', {0, 255, 0}}}, {255, 0, 0});
		
		entities.position[bullet] = entities.position[self];
		entities.velocity[bullet] = entities.velocity[self] + gun.bullet_speed * Vec2(entities.direction[self] + 0.0005 * (float)(rand() % 100 - 50));
		entities.direction[bullet] = entities.direction[self];
		
		entities.collision_effect[bullet] = [&entities](ecs::entity_t i){
			entities.health[i] -= 1;
		};
		entities.faction[bullet] = Faction::PLAYER;
		
		entities.health[bullet] = 2;
		
		// visual effect for bullet if it hits something
		entities.death_function[bullet] = [&entities](ecs::entity_t bullet) -> void{
			entities.timer[bullet] = 0.5;
			entities.image[bullet] = textures.load(explosion_pixels[0],
							{	{'a', {254, 254, 0}},
								{'b', {254, 0, 0}},
								{'c', {0, 254, 0}},
								{'d', {255, 255, 255}}	}, {255, 0, 0});
			entities.mask[bullet] = ecs::TIMER | ecs::IMAGE | ecs::POSITION;
			
		};
		
		entities.timer[bullet] = 2;
		entities.timer_function[bullet] = [&entities](ecs::entity_t self){entities.remove(self);};
	};
	return g;
}
Gun basic_gun(ecs::Entity & entities)
{
	Gun g;
	g.delay = 0.8;
	g.bullet_speed = 50;
	g.wait_time = 0;
	g.gun_function = [&entities](ecs::entity_t i)
	{
		ecs::entity_t bullet = entities.claim();
		entities.mask[bullet] = ecs::bullet_mask;
		
		entities.position[bullet] = entities.position[i];
		
		ecs::entity_t target = entities.target[i];
		Vec2 direction = entities.position[target] - entities.position[i];
		if(direction.norm() > 0)
			direction /= direction.norm();
		entities.velocity[bullet] = entities.velocity[i] + entities.gun[i].bullet_speed * direction;
		entities.image[bullet] = textures.load(bullet_pixels, {{'*', {255, 255, 0}}}, {255, 0, 0});
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
		if(entities.mask[i] == ecs::player_mask)
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

namespace ecs{
void Entity::spawn_player(void)
{
	ecs::entity_t player = claim();
	
	mask[player] = ecs::player_mask;
	
	image[player] = textures.load(player_pixels, {{'*', {255, 255, 255}}}, {255, 0, 0});
	
	int w, h;
	SDL_QueryTexture(image[player], nullptr, nullptr, &w, &h);
	Rect player_rect(0, 0, w, h);
	player_rect.setCenter(camera.getCenter());
	position[player] = player_rect.getCenter();
	velocity[player] = player_speed::normal * Vec2(0, -1);
	acceleration[player] = Vec2(0, 0);
	direction[player] = Vec2(0, -1).angle();
	size[player] = Size(w, h);
	
	gun[player] = player_gun(*this);
	
	health[player] = 3;
	
	collision_effect[player] = [this](ecs::entity_t i){
		health[i] -= 10;
	};
	
	faction[player] = Faction::PLAYER;
	
	think_function[player] = [this](ecs::entity_t self){keyboard_control(*this, self);};
	death_function[player] = [this](ecs::entity_t self){remove(self);};
}
}
ecs::Entity entities;
ecs::entity_t spawn_enemy(void)
{
	ecs::entity_t enemy = entities.claim();
	
	entities.mask[enemy] = ecs::enemy_mask;
	
	entities.image[enemy] = textures.load(enemy_pixels, {
		{'a', {0, 255, 255}},
		{'b', {254, 0, 0}},
		{'c', {0, 0, 250}},
		{'d', {255, 255, 0}}}, {255, 0, 0});
	
	int w, h;
	SDL_QueryTexture(entities.image[enemy], nullptr, nullptr, &w, &h);
	entities.position[enemy] = Vec2(rand() % window_width, camera.getTop() - h);
	entities.velocity[enemy] = Vec2(0, rand() % 50 + 1);
	entities.direction[enemy] = Vec2(0, 1).angle();
	entities.size[enemy] = Size(w, h);
	
	entities.gun[enemy] = basic_gun(entities);
	
	entities.health[enemy] = 3;
	
	entities.collision_effect[enemy] = [](ecs::entity_t i){
		entities.health[i] -= 2;
	};
	
	entities.faction[enemy] = Faction::ENEMY;
		
	entities.target[enemy] = get_player(entities);
	
	entities.death_function[enemy] = [](ecs::entity_t enemy) -> void
	{
		entities.health[enemy] = 1;
		entities.timer[enemy] = 0.5;
		entities.image[enemy] = textures.load(explosion_pixels[2],
							{{'a', {255, 254, 0}},
							{'b', {254, 0, 0}},
							{'c', {250, 0, 250}},
							{'d', {255, 255, 255}}}, {255, 0, 0});
		entities.mask[enemy] = ecs::IMAGE | ecs::TIMER | ecs::POSITION;
		entities.mask[enemy] |= ecs::TIMER;
		entities.timer_function[enemy] = [](ecs::entity_t enemy){entities.remove(enemy);};
	};
	return enemy;
}

ecs::entity_t spawn_enemy2(void)
{
	ecs::entity_t enemy = spawn_enemy();
	Size size = entities.size[enemy];
	entities.position[enemy] = Vec2(rand() % window_width, camera.getBottom() - size.h);
	entities.velocity[enemy] = Vec2(0, -player_speed::fast);
	entities.direction[enemy] = Vec2(0, -1).angle();
	entities.mask[enemy] |= ecs::THINK | ecs::ACCELERATION | ecs::TIMER;
	entities.think_function[enemy] = [](ecs::entity_t i){
		ecs::entity_t target = entities.target[i];
		entities.velocity[i].y = entities.velocity[target].y + (-50);
		entities.think_function[i] = [](ecs::entity_t i){
			
		};
	};
	entities.timer[enemy] = 10.0;
	entities.timer_function[enemy] = [](ecs::entity_t i){
		entities.think_function[i] = [](ecs::entity_t i){
			entities.acceleration[i] = Vec2(10, -50);
		};
	};
	return enemy;
}
void spawn_enemies(void)
{
	unsigned roll = rand();
	if(roll < 5)
		spawn_enemy2();
	else if(roll < 20)
		spawn_enemy();
}
void think_system(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(entities.mask[i] & ecs::THINK)
			entities.think_function[i](i);
	}
}
void shoot_system(float dt)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::shooter_mask) == ecs::shooter_mask)
		{
			// guns have a wait_time to wait before they can fire again
			if(entities.gun[i].wait_time > 0)
				entities.gun[i].wait_time -= dt;
			
			// spawn bullet if gun is ready
			if(entities.gun[i].fire && entities.gun[i].wait_time <= 0)
			{
				entities.gun[i].wait_time += entities.gun[i].delay;
				entities.gun[i].gun_function(i);
			}
		}
	}
}
void accelerate_system(float dt)
{
	ecs::mask_t accel_mask = ecs::VELOCITY | ecs::ACCELERATION;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & accel_mask) == accel_mask)
		{
			entities.velocity[i] += dt * entities.acceleration[i];
		}
	}
}
void move_system(float dt)
{
	ecs::mask_t move_mask = ecs::POSITION | ecs::VELOCITY;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & move_mask) == move_mask)
		{
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void collision_system(void)
{
	// first construct collision shape for entities that can collide
	struct Object{
		ecs::entity_t id;
		Circ shape;
	};
	std::vector<Object> players;
	std::vector<Object> enemies;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::collision_effect_mask) == ecs::collision_effect_mask)
		{
			Object obj;
			obj.id = i;
			obj.shape = Circ(entities.position[i], entities.size[i].w / 2.5);
			if(entities.faction[i] == Faction::PLAYER)
				players.push_back(obj);
			else if(entities.faction[i] == Faction::ENEMY)
				enemies.push_back(obj);
		}
	}
	
	// test the two groups against each other
	for(Object & i : players)
	{
		for(Object & j : enemies)
		{
			if(i.shape.intersects(j.shape))
			{
				entities.collision_effect[i.id](j.id);
				entities.collision_effect[j.id](i.id);
			}
		}
	}
}
void timer_system(float dt)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::TIMER) == ecs::TIMER)
		{
			entities.timer[i] -= dt;
			if(entities.timer[i] <= 0 && entities.timer_function[i] != nullptr)
				entities.timer_function[i](i);
		}
	}
}
void death_system(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		bool dead = ((entities.mask[i] & ecs::HEALTH) && entities.health[i] <= 0);
		if(dead && entities.death_function[i] != nullptr)
			entities.death_function[i](i);
	}
}
void update_camera(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::CAMERA_FOCUS) == ecs::CAMERA_FOCUS)
		{
			float center = entities.position[i].y;
			float adjustment = entities.velocity[i].y + player_speed::normal;
			float multiplier = window_height * 0.7 / (player_speed::fast - player_speed::slow);
			camera.setCenterY(floor(center - adjustment * multiplier));
			return;
		}
	}
			
}

void despawn_enemy(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::enemy_mask) == ecs::enemy_mask)
		{
			if(	(entities.position[i].y > camera.getBottom() &&
				entities.velocity[i].y > 0) ||
				(entities.position[i].y < camera.getTop() &&
				entities.velocity[i].y < 0))
				entities.remove(i);
		}
	}
}
void spawn_star(void)
{
	if((rand() % 1000) < 10 && stars.size() < 100)
	{
		stars.push_back(Vec2(rand() % window_width, camera.y - 10));
	}
}
void despawn_star(void)
{
	while((not stars.empty()) && stars.front().y > camera.getBottom())
	{
		stars.pop_front();
	}
}
void update(float const dt)
{
	think_system();
	shoot_system(dt);
	
	accelerate_system(dt);
	move_system(dt);
	
	collision_system();
	timer_system(dt);
	death_system();
	
	update_camera();
	
	spawn_enemies();
	despawn_enemy();
	spawn_star();
	despawn_star();
	
}
void draw(void)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// draw stars
	SDL_Texture * star_tex = textures.load(star_pixels, {{'*', {120, 120, 120}}}, {255, 0, 0});
	for(auto & i : stars)
	{
		SDL_Rect position;
		position.x = i.x - camera.x;
		position.y = i.y - camera.y;
		SDL_QueryTexture(star_tex, nullptr, nullptr, &position.w, &position.h);
		SDL_RenderCopy(renderer, star_tex, nullptr, &position);
	}
	
	// draw entities (space fighters and bullets)
	ecs::mask_t draw_mask = ecs::POSITION | ecs::IMAGE;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & draw_mask) == draw_mask)
		{
			SDL_Rect position;
			SDL_QueryTexture(entities.image[i], nullptr, nullptr, &position.w, &position.h);
			position.x = int(entities.position[i].x) - position.w / 2 - int(camera.x);
			position.y = int(entities.position[i].y) - position.h / 2 - int(camera.y);
			SDL_RenderCopyEx(renderer, entities.image[i], nullptr, &position, (entities.direction[i] + PI / 2.0) * 180 / PI, nullptr, SDL_FLIP_NONE);
		}
	}
	
	SDL_RenderPresent(renderer);
}
void init_system(void)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	
}
void init_state(void)
{
	screen = SDL_CreateWindow("shooter game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
	renderer = SDL_CreateRenderer(screen, -1, 0);
	
	camera = Rect(0, 0, window_width, window_height);
	
	textures = TextureStorage(renderer);
}


void cleanup_state(void)
{
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(screen);
	screen = nullptr;
}
void cleanup_system(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	SDL_Quit();
}
int main(int argc, char ** argv)
{
	init_system();
	init_state();
	
	entities.spawn_player();
	
	Uint32 frame_begin = 0, frame_end = 0;
	float dt = 0.017;
	static char title[50];
	while(not control::quit)
	{
		frame_begin = frame_end;
		
		handle_event();
		if(dt > 1.0 / 30.0)
			dt = 1.0 / 30.0;
		while(dt > 0)
		{
			update(dt_unit);
			dt -= dt_unit;
		}
		draw();
		
		frame_end = SDL_GetTicks();
		dt += (float)(frame_end - frame_begin) / 1000.0;
		snprintf(title, sizeof title,  "%f", dt);
		SDL_SetWindowTitle(screen, title);
	}
	cleanup_state();
	cleanup_system();
	return 0;
}
