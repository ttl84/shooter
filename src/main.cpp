#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <deque>
#include <stack>
#include <string>
#include <sstream>

#include <SDL2/SDL.h>

#include "Vec2.h"
#include "Size.h"
#include "Rect.h"
#include "Circ.h"
#include "PI.h"
#include "TextureStorage.h"
#include "CharImg.h"
CharImg const player_pixels("\
        *        \n\
        *        \n\
       * *       \n\
      ** **      \n\
      ** **      \n\
     **   **     \n\
     ** * **     \n\
     ** * **     \n\
      *****      \n\
     *******     \n\
    * ***** *    \n\
   *   ***   *   \n\
  *  * *** *  *  \n\
 ** *   *   * ** \n\
 ***   ***   *** \n\
  **         **  \n\
    *       *    ");

CharImg const enemy_pixels("\
        *        \n\
        *        \n\
       * *       \n\
      ** **      \n\
     *** ***     \n\
      ** **      \n\
       * *       \n\
      ** **      \n\
    **** ****    \n\
   ***** *****   \n\
  *** * * * ***  \n\
 **** * * * **** \n\
***    ***    ***\n\
**     ***     **\n\
 *      *      * \n\
 *      *      * \n\
        *        ");
CharImg const bullet_pixels("\
*\n\
*");

CharImg const star_pixels("*");

struct Gun{
	float delay; // time to wait after every fire
	float bullet_speed;
	
	float wait_time; // time left before the next fire
	Vec2 direction;
	bool fire;
	Gun(void) : delay(0), bullet_speed(0), wait_time(0), fire(false) {}
};
Gun player_gun(void)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 450;
	g.wait_time = 0;
	return g;
}

Gun basic_gun(void)
{
	Gun g;
	g.delay = 0.8;
	g.bullet_speed = 50;
	g.wait_time = 0;
	return g;
}

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
		LIFESPAN = BIT(7),
		COLLISION_DAMAGE = BIT(8),
		
		ENEMY_CONTROL = BIT(9),
		KEYBOARD_CONTROL = BIT(10),
		
		FACTION = BIT(11),
		GUN = BIT(12),
		CAMERA_FOCUS = BIT(13)
		
	};
	mask_t constexpr move_mask = POSITION | VELOCITY;
	mask_t constexpr collision_damage_mask = POSITION | SIZE | COLLISION_DAMAGE | FACTION;
	mask_t constexpr shooter_mask = POSITION | DIRECTION | SIZE | GUN;
	mask_t constexpr player_mask = move_mask | ACCELERATION | shooter_mask | collision_damage_mask | IMAGE | HEALTH | KEYBOARD_CONTROL | CAMERA_FOCUS;
	mask_t constexpr enemy_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | ENEMY_CONTROL | HEALTH;
	mask_t constexpr bullet_mask = move_mask | collision_damage_mask | IMAGE | LIFESPAN | HEALTH;
	
	template <unsigned MAX_ENTITIES>
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
		float lifespan[MAX_ENTITIES];
		Gun gun[MAX_ENTITIES];
		Faction faction[MAX_ENTITIES];
		
		Entity(void): mask{0}, image{NULL}, m_count(0) {}
		entity_t claim(void)
		{
			
			if(holes.empty())
			{
				if(m_count == MAX_ENTITIES)
					return m_count - 1;
				else
					return m_count++;
			}
			else
			{
				entity_t hole = holes.top();
				holes.pop();
				return hole;
			}
		}
		void remove(entity_t i)
		{
			mask[i] = 0;
			image[i] = NULL;
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

bool running = true;
float const dt_unit = 1.0 / 1000.0;
unsigned window_width = 640;
unsigned window_height = 640;

SDL_Window * screen = NULL;
SDL_Renderer * renderer = NULL;
Rect camera;


ecs::Entity<1000> entities;
std::deque<Vec2> stars;

TextureStorage textures;

namespace control{
	bool faster = false;
	bool slower = false;
	bool left = false;
	bool right = false;
	bool fire = false;
}

namespace player_speed{
	float fast = 350;
	float slow = 50;
	float normal = (fast + slow) / 2.0;
}


void handle_event(void)
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_KEYDOWN)
		{
			SDL_Keycode symbol = e.key.keysym.sym;
			if(symbol == SDLK_UP)
				control::faster = true;
			else if(symbol == SDLK_DOWN)
				control::slower = true;
			else if(symbol == SDLK_LEFT)
				control::left = true;
			else if(symbol == SDLK_RIGHT)
				control::right = true;
			else if(symbol == SDLK_z)
				control::fire = true;
			else if(symbol == SDLK_ESCAPE)
				running = false;
				
		}
		else if(e.type == SDL_KEYUP)
		{
			SDL_Keycode symbol = e.key.keysym.sym;
			if(symbol == SDLK_UP)
				control::faster = false;
			else if(symbol == SDLK_DOWN)
				control::slower = false;
			else if(symbol == SDLK_LEFT)
				control::left = false;
			else if(symbol == SDLK_RIGHT)
				control::right = false;
			else if(symbol == SDLK_z)
				control::fire = false;
		}
		else if(e.type == SDL_QUIT)
		{
			running = false;
		}
	}
}
void spawn_player(void)
{
	ecs::entity_t player = entities.claim();
	
	entities.mask[player] = ecs::player_mask;
	
	entities.image[player] = textures.load(player_pixels, {{'*', {255, 255, 255}}}, {255, 0, 0});
	
	int w, h;
	SDL_QueryTexture(entities.image[player], NULL, NULL, &w, &h);
	Rect player_rect(0, 0, w, h);
	player_rect.setCenter(camera.getCenter());
	entities.position[player] = player_rect.getPosition();
	entities.velocity[player] = player_speed::normal * Vec2(0, -1);
	entities.acceleration[player] = Vec2(0, 0);
	entities.direction[player] = Vec2(0, -1).angle();
	entities.size[player] = Size(w, h);
	
	entities.gun[player] = player_gun();
	
	entities.health[player] = 3;
	
	entities.collision_damage[player] = 10;
	
	entities.faction[player] = Faction::PLAYER;
}
void spawn_enemy(void)
{
	if((rand() % 1000)  == 0)
	{
		ecs::entity_t enemy = entities.claim();
		
		entities.mask[enemy] = ecs::enemy_mask;
		
		entities.image[enemy] = textures.load(enemy_pixels, {{'*', {0, 0, 255}}}, {255, 0, 0});
		
		int w, h;
		SDL_QueryTexture(entities.image[enemy], NULL, NULL, &w, &h);
		entities.position[enemy] = Vec2(rand() % window_width, camera.getTop() - h);
		entities.velocity[enemy] = Vec2(0, rand() % 50 + 1);
		entities.direction[enemy] = Vec2(0, 1).angle();
		entities.size[enemy] = Size(w, h);
		
		entities.gun[enemy] = basic_gun();
		
		entities.health[enemy] = 3;
		
		entities.collision_damage[enemy] = 2;
		
		entities.faction[enemy] = Faction::ENEMY;
	}
}
void keyboard_control(void)
{
	// finds the entity with keyboard control privilege
	ecs::entity_t i = 0;
	for(i = 0; i < entities.count(); i++)
		if((entities.mask[i] & ecs::KEYBOARD_CONTROL) == ecs::KEYBOARD_CONTROL)
			break;
	if(i == entities.count())
		return;
	
	float speed = entities.velocity[i].norm();
	
	// makes sure player  doesn't move backwards
	if(fabs(entities.velocity[i].y) > player_speed::fast)
		entities.velocity[i].y = player_speed::fast * -1;
	if(fabs(entities.velocity[i].y) < player_speed::slow)
		entities.velocity[i].y = player_speed::slow * -1;
	
	
	if(control::faster)
	{
		if(speed < player_speed::fast)
			entities.acceleration[i].y = -100;
		else
			entities.acceleration[i].y = 0;
	}
	else if(control::slower)
	{
		if(speed > player_speed::slow)
			entities.acceleration[i].y = 100;
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
		entities.acceleration[i].x = -2 * entities.velocity[i].x;
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
				
				unsigned bullet = entities.claim();
				entities.mask[bullet] = ecs::bullet_mask;
				
				entities.position[bullet] = entities.position[i];
				
				entities.velocity[bullet] = entities.velocity[i] + entities.gun[i].bullet_speed * Vec2(entities.direction[i]);
				entities.image[bullet] = textures.load(bullet_pixels, {{'*', {255, 255, 0}}}, {255, 0, 0});
				entities.lifespan[bullet] = 4.0;
				entities.health[bullet] = 1;
				entities.collision_damage[bullet] = 1;
				entities.faction[bullet] = entities.faction[i];
			}
		}
	}
}
void bullet_process(float dt)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::bullet_mask) == ecs::bullet_mask)
		{
			entities.lifespan[i] -= dt;
		}
	}
}
void move_process(float dt)
{
	
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::move_mask) == ecs::move_mask)
		{
			entities.velocity[i] += dt * entities.acceleration[i];
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void collision_damage_process(void)
{
	std::vector<ecs::entity_t> players;
	std::vector<ecs::entity_t> enemies;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::collision_damage_mask) == ecs::collision_damage_mask)
		{
			if(entities.faction[i] == Faction::PLAYER)
				players.push_back(i);
			else if(entities.faction[i] == Faction::ENEMY)
				enemies.push_back(i);
		}
	}
	for(ecs::entity_t i : players)
	{
		Circ shapeA(entities.position[i], entities.size[i].w / 2.5);
		for(ecs::entity_t j : enemies)
		{
			Circ shapeB(entities.position[j], entities.size[j].w / 2.5);
			if(shapeA.intersects(shapeB))
			{
				entities.health[i] -= entities.collision_damage[j];
				entities.health[j] -= entities.collision_damage[i];
			}
		}
	}
}
void death_process(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(((entities.mask[i] & ecs::HEALTH) == ecs::HEALTH && entities.health[i] <= 0) ||
			((entities.mask[i] & ecs::LIFESPAN) == ecs::LIFESPAN && entities.lifespan[i] <= 0))
				entities.remove(i);
	}
}
void update_camera(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::CAMERA_FOCUS) == ecs::CAMERA_FOCUS)
		{
			camera.setCenterY(floor(entities.position[i].y - entities.velocity[i].y - player_speed::normal));
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
			if(entities.position[i].y > camera.getBottom())
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
	keyboard_control();
	shooter_process(dt);
	bullet_process(dt);
	move_process(dt);
	collision_damage_process();
	death_process();
	
	update_camera();
	
	spawn_enemy();
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
	for(auto i = stars.begin(); i != stars.end(); ++i)
	{
		SDL_Rect position;
		position.x = i->x - camera.x;
		position.y = i->y - camera.y;
		SDL_QueryTexture(star_tex, NULL, NULL, &position.w, &position.h);
		SDL_RenderCopy(renderer, star_tex, NULL, &position);
	}
	
	// draw entities (space fighters and bullets)
	ecs::mask_t draw_mask = ecs::POSITION | ecs::IMAGE;
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & draw_mask) == draw_mask)
		{
			SDL_Rect position;
			SDL_QueryTexture(entities.image[i], NULL, NULL, &position.w, &position.h);
			position.x = int(entities.position[i].x) - position.w / 2 - int(camera.x);
			position.y = int(entities.position[i].y) - position.h / 2 - int(camera.y);
			SDL_RenderCopyEx(renderer, entities.image[i], NULL, &position, (entities.direction[i] + PI / 2.0) * 180 / PI, NULL, SDL_FLIP_NONE);
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
	renderer = NULL;
	SDL_DestroyWindow(screen);
	screen = NULL;
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
	while(running)
	{
		frame_begin = frame_end;
		
		handle_event();
		if(dt > 0.030)
			dt = 0.020;
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
