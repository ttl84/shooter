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
		entities.velocity[bullet] = entities.velocity[self] + gun.bullet_speed * Vec2::fromAngle(entities.direction[self] + 0.0005 * (float)(rand() % 100 - 50));
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

void spawn_player(ecs::Entity & entities)
{
	ecs::entity_t player = entities.claim();
	
	entities.mask[player] = ecs::player_mask;
	
	entities.image[player] = textures.load(player_pixels, {{'*', {255, 255, 255}}}, {255, 0, 0});
	
	int w, h;
	SDL_QueryTexture(entities.image[player], nullptr, nullptr, &w, &h);
	Rect player_rect(0, 0, w, h);
	player_rect.setCenter(camera.getCenter());
	entities.position[player] = player_rect.getCenter();
	entities.velocity[player] = player_speed::normal * Vec2(0, -1);
	entities.acceleration[player] = Vec2(0, 0);
	entities.direction[player] = Vec2(0, -1).angle();
	entities.size[player] = Size(w, h);
	
	entities.gun[player] = player_gun(entities);
	
	entities.health[player] = 3;
	
	entities.collision_effect[player] = [&entities](ecs::entity_t i){
		entities.health[i] -= 10;
	};
	
	entities.faction[player] = Faction::PLAYER;
	
	entities.think_function[player] = [&entities](ecs::entity_t self){keyboard_control(entities, self);};
	entities.death_function[player] = [&entities](ecs::entity_t self){entities.remove(self);};
}
ecs::entity_t spawn_enemy(ecs::Entity & entities)
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
	
	entities.collision_effect[enemy] = [&entities](ecs::entity_t i){
		entities.health[i] -= 2;
	};
	
	entities.faction[enemy] = Faction::ENEMY;
		
	entities.target[enemy] = get_player(entities);
	
	entities.death_function[enemy] = [&entities](ecs::entity_t enemy) -> void
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
		entities.timer_function[enemy] = [&entities](ecs::entity_t enemy){entities.remove(enemy);};
	};
	return enemy;
}

ecs::entity_t spawn_enemy2(ecs::Entity & entities)
{
	ecs::entity_t enemy = spawn_enemy(entities);
	Size size = entities.size[enemy];
	entities.position[enemy] = Vec2(rand() % window_width, camera.getBottom() - size.h);
	entities.velocity[enemy] = Vec2(0, -player_speed::fast);
	entities.direction[enemy] = Vec2(0, -1).angle();
	entities.mask[enemy] |= ecs::THINK | ecs::ACCELERATION | ecs::TIMER;
	entities.think_function[enemy] = [&entities](ecs::entity_t i){
		ecs::entity_t target = entities.target[i];
		entities.velocity[i].y = entities.velocity[target].y + (-50);
		entities.think_function[i] = [](ecs::entity_t i){
			
		};
	};
	entities.timer[enemy] = 10.0;
	entities.timer_function[enemy] = [&entities](ecs::entity_t i){
		entities.think_function[i] = [&entities](ecs::entity_t i){
			entities.acceleration[i] = Vec2(10, -50);
		};
	};
	return enemy;
}

void spawn_enemies(ecs::Entity & entities)
{
	unsigned roll = rand();
	if(roll < 5)
		spawn_enemy2(entities);
	else if(roll < 20)
		spawn_enemy(entities);
}



void update_camera(ecs::Entity & entities)
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

void despawn_enemy(ecs::Entity & entities)
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
	while((not stars.empty()) and (stars.front().y > camera.getBottom()))
	{
		stars.pop_front();
	}
}
void update(ecs::Entity & entities, float const dt)
{
	entities.thinkSystem();
	entities.shootSystem(dt);
	
	entities.accelSystem(dt);
	entities.moveSystem(dt);
	
	entities.collisionSystem();
	entities.timerSystem(dt);
	entities.deathSystem();
	
	update_camera(entities);
	
	spawn_enemies(entities);
	despawn_enemy(entities);
	spawn_star();
	despawn_star();
	
}
void draw_stars(void)
{
	SDL_Texture * star_tex = textures.load(star_pixels, {{'*', {120, 120, 120}}}, {255, 0, 0});
	for(auto & i : stars)
	{
		SDL_Rect position;
		position.x = i.x - camera.x;
		position.y = i.y - camera.y;
		SDL_QueryTexture(star_tex, nullptr, nullptr, &position.w, &position.h);
		SDL_RenderCopy(renderer, star_tex, nullptr, &position);
	}
}

void draw(ecs::Entity & entities)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	draw_stars();
	entities.drawSystem(renderer, camera);
	
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
	
	static ecs::Entity entities;
	
	spawn_player(entities);
	
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
			update(entities, dt_unit);
			dt -= dt_unit;
		}
		draw(entities);
		
		frame_end = SDL_GetTicks();
		dt += (float)(frame_end - frame_begin) / 1000.0;
		snprintf(title, sizeof title,  "%f", dt);
		SDL_SetWindowTitle(screen, title);
	}
	cleanup_state();
	cleanup_system();
	return 0;
}
