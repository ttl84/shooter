#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <deque>
#include <stack>
#include <string>
#include <SDL2/SDL.h>

#include "Vec2.h"
#include "Size.h"
#include "Rect.h"
#include "PI.h"
#include "TextureStorage.h"
std::string const player_pixels = "\
        *        \n\
        *        \n\
       * *       \n\
       * *       \n\
       * *       \n\
       * *       \n\
      * * *      \n\
      * * *      \n\
     * * * *     \n\
     *     *     \n\
    * *   * *    \n\
   ** *   * **   \n\
 *  * *   * *  * \n\
**  * *   * *  **\n\
   **  ***  **   \n\
   **  ***  **   ";

std::string const enemy_pixels = "\
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
        *        ";
std::string const bullet_pixels = "\
*\n\
*";

std::string const star_pixels = "*";

struct Gun{
	bool fire;
	float wait_time;
	float delay;
	enum class Side{
		CENTER,
		LEFT,
		RIGHT
	}side;
	Vec2 direction;
	float bullet_speed;
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
		DIRECTION = BIT(2),
		SIZE = BIT(3),
		
		IMAGE = BIT(4),
		
		HEALTH = BIT(5),
		LIFESPAN = BIT(6),
		COLLISION_DAMAGE = BIT(7),
		
		ENEMY_CONTROL = BIT(8),
		KEYBOARD_CONTROL = BIT(9),
		
		FACTION = BIT(10),
		GUN = BIT(11),
		CAMERA_FOCUS = BIT(12)
		
	};
	mask_t constexpr move_mask = POSITION | VELOCITY;
	mask_t constexpr collision_damage_mask = POSITION | SIZE | COLLISION_DAMAGE | FACTION;
	mask_t constexpr shooter_mask = POSITION | DIRECTION | SIZE | GUN;
	mask_t constexpr player_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | HEALTH | KEYBOARD_CONTROL | CAMERA_FOCUS;
	mask_t constexpr enemy_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | ENEMY_CONTROL | HEALTH;
	mask_t constexpr bullet_mask = move_mask | collision_damage_mask | IMAGE | LIFESPAN | HEALTH;
	
	template <unsigned MAX_ENTITIES>
	struct Entity{
		mask_t mask[MAX_ENTITIES];
		Vec2 position[MAX_ENTITIES];
		Vec2 velocity[MAX_ENTITIES];
		double direction[MAX_ENTITIES];
		Size size[MAX_ENTITIES];
		SDL_Texture * image[MAX_ENTITIES];
		int collision_damage[MAX_ENTITIES];
		int health[MAX_ENTITIES];
		float lifespan[MAX_ENTITIES];
		Gun gun[MAX_ENTITIES];
		Faction faction[MAX_ENTITIES];
		
		Entity(void): mask({0}), image({NULL}), m_count(0)
		{
		}
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
unsigned window_width = 320;
unsigned window_height = 320;

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
	float normal = 20;
	float fast = 50;
	float slow = 10;
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
	
	entities.image[player] = textures.load(player_pixels);
	
	int w, h;
	SDL_QueryTexture(entities.image[player], NULL, NULL, &w, &h);
	Rect player_rect(0, 0, w, h);
	player_rect.setCenter(camera.getCenter());
	entities.position[player] = player_rect.getPosition();
	entities.direction[player] = Vec2(0, -1).angle();
	entities.size[player] = Size(w, h);
	
	entities.gun[player].side = Gun::Side::LEFT;
	entities.gun[player].delay = 0.1;
	entities.gun[player].direction = Vec2(0, -1);
	entities.gun[player].bullet_speed = 100;
	
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
		
		entities.image[enemy] = textures.load(enemy_pixels);
		
		int w, h;
		SDL_QueryTexture(entities.image[enemy], NULL, NULL, &w, &h);
		entities.position[enemy] = Vec2(rand() % window_width, camera.getTop() - h);
		entities.velocity[enemy] = Vec2(0, rand() % 50 + 1);
		entities.direction[enemy] = Vec2(0, 1).angle();
		entities.size[enemy] = Size(w, h);
		
		entities.gun[enemy].side = Gun::Side::CENTER;
		entities.gun[enemy].delay = 1.0 / (float)(rand() % 3 + 1);
		entities.gun[enemy].fire = true;
		entities.gun[enemy].bullet_speed = 50;
		
		entities.health[enemy] = 3;
		
		entities.collision_damage[enemy] = 2;
		
		entities.faction[enemy] = Faction::ENEMY;
	}
}
void keyboard_control(void)
{
	// finds the entity with keyboard control privilege
	ecs::entity_t i = 0;
	while(not (entities.mask[i] & ecs::KEYBOARD_CONTROL) == ecs::KEYBOARD_CONTROL)
		i++;
	
	float speed;
	if(control::faster)
	{
		speed = player_speed::fast;
	}
	else if(control::slower)
	{
		speed = player_speed::slow;
	}
	else
	{
		speed = player_speed::normal;
	}
	
	if(control::left)
	{
		entities.velocity[i].x = -1;
	}
	else if(control::right)
	{
		entities.velocity[i].x = 1;
	}
	else
	{
		entities.velocity[i].x = 0;
	}
	entities.velocity[i].y = -1;
	entities.velocity[i] /= entities.velocity[i].norm();
	entities.velocity[i] *= speed;
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
				
				
				// alternating fire
				if(entities.gun[i].side == Gun::Side::LEFT)
				{
					entities.position[bullet] = entities.position[i];
					entities.gun[i].side = Gun::Side::RIGHT;
				}
				else if(entities.gun[i].side == Gun::Side::RIGHT)
				{
					entities.position[bullet] = entities.position[i] + Vec2(entities.size[i].w, 0);
					entities.gun[i].side = Gun::Side::LEFT;
				}
				
				// concentrated fire
				else if(entities.gun[i].side == Gun::Side::CENTER)
				{
					entities.position[bullet] = entities.position[i] + Vec2(entities.size[i].w / 2, 0);
				}
				
				entities.velocity[bullet] = entities.velocity[i] + entities.gun[i].bullet_speed * Vec2(entities.direction[i]);
				entities.image[bullet] = textures.load(bullet_pixels);
				entities.lifespan[bullet] = 5.0;
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
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void collision_damage_process(void)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::collision_damage_mask) == ecs::collision_damage_mask)
		{
			Rect rectA(entities.position[i], entities.size[i]);
			for(ecs::entity_t j = i + 1; j < entities.count(); j++)
			{
				if((entities.mask[j] & ecs::collision_damage_mask) == ecs::collision_damage_mask)
				{
					Rect rectB(entities.position[j], entities.size[j]);
					if(rectA.intersects(rectB) && entities.faction[i] != entities.faction[j])
					{
						entities.health[i] -= entities.collision_damage[j];
						entities.health[j] -= entities.collision_damage[i];
					}
				}
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
			camera.setCenterY(floor(entities.position[i].y));
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
	while(stars.front().y > camera.getBottom())
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
	SDL_Texture * star_tex = textures.load(star_pixels);
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
			position.x = entities.position[i].x - camera.x;
			position.y = entities.position[i].y - camera.y;
			SDL_QueryTexture(entities.image[i], NULL, NULL, &position.w, &position.h);
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
	while(running)
	{
		frame_begin = frame_end;
		
		handle_event();
		while(dt > 0)
		{
			update(dt_unit);
			dt -= dt_unit;
		}
		draw();
		
		frame_end = SDL_GetTicks();
		dt += (float)(frame_end - frame_begin) / 1000.0;
		if(dt > 0.030)
			dt = 0.020;
	}
	cleanup_state();
	cleanup_system();
	return 0;
}
