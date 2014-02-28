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

#include "Vec2.h"
#include "Size.h"
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

struct Gun{
	float delay; // time to wait after every fire
	float bullet_speed;
	
	float wait_time; // time left before the next fire
	bool fire;
	std::function<void(unsigned)> gun_function;
	Gun(void) : delay(0), bullet_speed(0), wait_time(0), fire(false), gun_function(nullptr) {}
};


enum class Faction{
	PLAYER,
	ENEMY
};

// entity component system implementation based on BorealisGames's
namespace ecs{
	typedef uint16_t mask_t;
	typedef unsigned entity_t;
	unsigned constexpr BIT(unsigned n)
	{
		return 1 << n;
	}
	enum flag{
		POSITION = BIT(0),
		VELOCITY = BIT(1),
		ACCELERATION = BIT(2),
		DIRECTION = BIT(3),
		SIZE = BIT(4),
		
		IMAGE = BIT(5),
		
		HEALTH = BIT(6),
		TIMER = BIT(7),
		COLLISION_DAMAGE = BIT(8),
		
		THINK = BIT(9),
		
		FACTION = BIT(10),
		GUN = BIT(11),
		CAMERA_FOCUS = BIT(12),
		TARGET = BIT(13)
		
	};
	mask_t constexpr move_mask = POSITION | VELOCITY;
	mask_t constexpr collision_damage_mask = POSITION | SIZE | COLLISION_DAMAGE | FACTION;
	mask_t constexpr shooter_mask = POSITION | DIRECTION | SIZE | GUN;
	mask_t constexpr player_mask = move_mask | ACCELERATION | shooter_mask | collision_damage_mask | IMAGE | HEALTH | THINK | CAMERA_FOCUS;
	mask_t constexpr enemy_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | HEALTH;
	mask_t constexpr bullet_mask = move_mask | collision_damage_mask | DIRECTION | IMAGE | TIMER | HEALTH;
	
	constexpr unsigned MAX_ENTITIES = 1000;
	struct Entity{
		mask_t mask[MAX_ENTITIES];
		Vec2 position[MAX_ENTITIES];
		Vec2 velocity[MAX_ENTITIES];
		Vec2 acceleration[MAX_ENTITIES];
		double direction[MAX_ENTITIES];
		Size size[MAX_ENTITIES];
		SDL_Texture * image[MAX_ENTITIES];
		int collision_damage[MAX_ENTITIES];
		int health[MAX_ENTITIES];
		float timer[MAX_ENTITIES];
		entity_t target[MAX_ENTITIES];
		
		Gun gun[MAX_ENTITIES];
		Faction faction[MAX_ENTITIES];
		
		std::function<void(entity_t)> think_function[MAX_ENTITIES];
		std::function<void(entity_t)> death_function[MAX_ENTITIES];
		std::function<void(entity_t)> timer_function[MAX_ENTITIES];
		std::function<void(entity_t)> despawn_function[MAX_ENTITIES];
		
		Entity(void): mask{0}, image{nullptr}, m_count(0) {}
		entity_t claim(void)
		{
			entity_t next;
			if(holes.empty())
			{
				if(m_count == MAX_ENTITIES)
				{
					std::cerr << "out of slots for new entity, aborting\n";
					abort();
				}
				else
					next = m_count++;
			}
			else
			{
				next = holes.top();
				holes.pop();
			}
			image[next] = nullptr;
			target[next] = 0;
			think_function[next] = [](entity_t self){std::cerr << "missing think function\n";};
			death_function[next] = [](entity_t self){std::cerr << "missing death function\n";};
			timer_function[next] = [](entity_t self){std::cerr << "missing timer function\n";};
			return next;
		}
		void remove(entity_t i)
		{
			mask[i] = 0;
			holes.push(i);
		}
		unsigned count(void) const
		{
			return m_count;
		}
		unsigned capacity(void) const
		{
			return MAX_ENTITIES;
		}
	private:
		unsigned m_count;
		std::stack<entity_t> holes;
	};
}

float const dt_unit = 1.0 / 1000.0;
unsigned window_width = 640;
unsigned window_height = 480;

SDL_Window * screen = nullptr;
SDL_Renderer * renderer = nullptr;
Rect camera;


ecs::Entity entities;
std::deque<Vec2> stars;

TextureStorage textures;



namespace player_speed{
	float fast = 200;
	float slow = 50;
	float normal = slow + (fast - slow) / 2.0;
}
Gun player_gun(void)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 550;
	g.wait_time = 0;
	g.gun_function = [](ecs::entity_t self) -> void
	{
		
		Gun const & gun = entities.gun[self];
		
		ecs::entity_t bullet = entities.claim();
		entities.mask[bullet] = ecs::bullet_mask;
		
		entities.image[bullet] = textures.load(laser_pixels, {{'*', {0, 255, 0}}}, {255, 0, 0});
		
		entities.position[bullet] = entities.position[self];
		entities.velocity[bullet] = entities.velocity[self] + gun.bullet_speed * Vec2(entities.direction[self] + 0.0005 * (float)(rand() % 100 - 50));
		entities.direction[bullet] = entities.direction[self];
		
		entities.collision_damage[bullet] = 1;
		entities.faction[bullet] = Faction::PLAYER;
		
		entities.health[bullet] = 2;
		
		// visual effect for bullet if it hits something
		entities.death_function[bullet] = [](ecs::entity_t bullet) -> void{
			entities.timer[bullet] = 0.5;
			entities.image[bullet] = textures.load(explosion_pixels[0],
							{	{'a', {254, 254, 0}},
								{'b', {254, 0, 0}},
								{'c', {0, 254, 0}},
								{'d', {255, 255, 255}}	}, {255, 0, 0});
			entities.mask[bullet] = ecs::TIMER | ecs::IMAGE | ecs::POSITION;
			
		};
		
		entities.timer[bullet] = 2;
		entities.timer_function[bullet] = [](ecs::entity_t self){entities.remove(self);};
	};
	return g;
}

Gun basic_gun(void)
{
	Gun g;
	g.delay = 0.8;
	g.bullet_speed = 50;
	g.wait_time = 0;
	g.gun_function = [](ecs::entity_t i)
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
		entities.timer_function[bullet] = [](ecs::entity_t i){entities.health[i] = 0;};
		
		entities.health[bullet] = 1;
		entities.death_function[bullet] = [](ecs::entity_t i){entities.remove(i);};
		
		entities.collision_damage[bullet] = 1;
		entities.faction[bullet] = entities.faction[i];
		
	};
	g.fire = true;
	return g;
}
ecs::entity_t get_player(void)
{
	ecs::entity_t i;
	for(i = 0; i < entities.count(); i++)
		if(entities.mask[i] == ecs::player_mask)
			break;
	return i;
}
void keyboard_control(ecs::entity_t i)
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
void spawn_player(void)
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
	
	entities.gun[player] = player_gun();
	
	entities.health[player] = 3;
	
	entities.collision_damage[player] = 10;
	
	entities.faction[player] = Faction::PLAYER;
	
	entities.think_function[player] = keyboard_control;
	entities.death_function[player] = [](ecs::entity_t self){entities.remove(self);};
}
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
	
	entities.gun[enemy] = basic_gun();
	
	entities.health[enemy] = 3;
	
	entities.collision_damage[enemy] = 2;
	
	entities.faction[enemy] = Faction::ENEMY;
		
	entities.target[enemy] = get_player();
	
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
		if(entities.position[i].y + 50 > entities.position[target].y)
			entities.acceleration[i] = Vec2(0, -50);
		else if(entities.velocity[i].y - 50 < entities.velocity[target].y)
			entities.acceleration[i] = Vec2(0, 100);
		else
			entities.acceleration[i] = Vec2{0, 0};
	};
	entities.timer_function[enemy] = [](ecs::entity_t i){
	return enemy;
}
void spawn_enemies(void)
{
	unsigned roll = rand();
	if(roll < 10)
		spawn_enemy2();
	else if(roll < 20)
		spawn_enemy();
}
void think_process(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(entities.mask[i] & ecs::THINK)
			entities.think_function[i](i);
	}
}
void shooter_process(float dt)
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

void move_process(float dt)
{
	
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		ecs::mask_t myMask = entities.mask[i];
		if((myMask & ecs::POSITION) && (myMask & ecs::VELOCITY))
		{
			if(myMask & ecs::ACCELERATION)
				entities.velocity[i] += dt * entities.acceleration[i];
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void collision_damage_process(void)
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
		if((entities.mask[i] & ecs::collision_damage_mask) == ecs::collision_damage_mask)
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
				entities.health[i.id] -= entities.collision_damage[j.id];
				entities.health[j.id] -= entities.collision_damage[i.id];
			}
		}
	}
}
void timer_process(float dt)
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
void death_process(void)
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
	think_process();
	shooter_process(dt);
	
	move_process(dt);
	
	collision_damage_process();
	timer_process(dt);
	death_process();
	
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
	
	spawn_player();
	
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
