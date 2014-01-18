#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <deque>
#include <map>
#include <stack>
#include <iostream>
#include <SDL/SDL.h>

#include "Vec2.h"
#include "Size.h"
#include "Rect.h"
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
     * * * *     \n\
   ** *   * **   \n\
   ** *   * **   \n\
**  * *   * *  **\n\
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




// Rect represents a rectangle with top left corner as (x, y)
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
		IMAGE = BIT(2),
		HEALTH = BIT(3),
		ENEMY_CONTROL = BIT(4),
		COLLISION_DAMAGE = BIT(5),
		LIFESPAN = BIT(6),
		GUN = BIT(7),
		SIZE = BIT(8),
		FACTION = BIT(9),
		KEYBOARD_CONTROL = BIT(10),
		CAMERA_FOCUS = BIT(11)
	};
	mask_t constexpr move_mask = POSITION | VELOCITY;
	mask_t constexpr collision_damage_mask = POSITION | SIZE | COLLISION_DAMAGE | FACTION;
	mask_t constexpr shooter_mask = POSITION | SIZE | GUN;
	mask_t constexpr player_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | HEALTH | KEYBOARD_CONTROL | CAMERA_FOCUS;
	mask_t constexpr enemy_mask = move_mask | shooter_mask | collision_damage_mask | IMAGE | ENEMY_CONTROL | HEALTH;
	mask_t constexpr bullet_mask = move_mask | collision_damage_mask | IMAGE | LIFESPAN | HEALTH;
	
	template <unsigned MAX_ENTITIES>
	struct Entity{
		mask_t mask[MAX_ENTITIES];
		Vec2 position[MAX_ENTITIES];
		Vec2 velocity[MAX_ENTITIES];
		Size size[MAX_ENTITIES];
		SDL_Surface * image[MAX_ENTITIES];
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
			SDL_FreeSurface(image[i]);
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



SDL_Surface * surface_from_string(std::string const & pixels, SDL_PixelFormat const * format)
{
	unsigned width = 0;
	for(unsigned i = 0; i < pixels.length(); i++)
	{
		if(pixels[i] == '\n')
			break;
		else
			width++;
	}
	
	unsigned height = 1;
	for(unsigned i = 0; i < pixels.length(); i++)
	{
		if(pixels[i] == '\n')
			height++;
	}
	
	SDL_Surface * surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(surface->format, 255, 0, 0));

	if(surface == NULL)
		return NULL;
	Uint32 * data = (Uint32*)(surface->pixels);
	
	SDL_LockSurface(surface);
	unsigned i = 0;
	for(auto iter = pixels.begin(); i < width * height && iter != pixels.end(); ++iter)
	{
		if(*iter != '\n')
		{
			if(*iter == '*')
				data[i] = SDL_MapRGB(format, 255, 255, 255);
			else
				data[i] = SDL_MapRGB(format, 255, 0, 0);
			i++;
		}
	}
	SDL_UnlockSurface(surface);
	
	return surface;
}
SDL_Surface * surface_rotate_180(SDL_Surface const * surface)
{
	SDL_PixelFormat const * format = surface->format;
	SDL_Surface * rotated = SDL_CreateRGBSurface(SDL_HWSURFACE, surface->w, surface->h, 32, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	SDL_SetColorKey(rotated, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(rotated->format, 255, 0, 0));
	
	SDL_LockSurface(rotated);
	for(unsigned i = 0, end = surface->w * surface->h; i < end; i++)
	{
		((Uint32*)(rotated->pixels))[i] = ((Uint32*)(surface->pixels))[end-i-1];
	}
	SDL_UnlockSurface(rotated);
	return rotated;
}
bool running = true;
float const dt_unit = 1.0 /1000.0;
unsigned window_width = 320;
unsigned window_height = 320;

SDL_Surface * screen = NULL;
Rect camera;


ecs::Entity<1000> entities;
std::deque<Vec2> stars;

std::map<std::string, SDL_Surface*> surface_cache;


namespace control{
	bool faster = false;
	bool slower = false;
	bool left = false;
	bool right = false;
	bool fire = false;
}




void handle_event(void)
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_KEYDOWN)
		{
			SDLKey symbol = e.key.keysym.sym;
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
			SDLKey symbol = e.key.keysym.sym;
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
	
	entities.image[player] = surface_from_string(player_pixels, screen->format);
	
	Rect player_rect(0, 0, entities.image[player]->w, entities.image[player]->h);
	player_rect.setCenter(camera.getCenter());
	entities.position[player] = player_rect.getPosition();
	
	entities.size[player] = Size(entities.image[player]->w, entities.image[player]->h);
	
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
		
		SDL_Surface * enemy_image = surface_from_string(enemy_pixels, screen->format);
		entities.image[enemy] = surface_rotate_180(enemy_image);
		SDL_FreeSurface(enemy_image);
		
		entities.position[enemy] = Vec2(rand() % window_width, camera.getTop() - entities.image[enemy]->h);
		
		entities.velocity[enemy] = Vec2(0, rand() % 50 + 1);
		
		entities.size[enemy] = Size(entities.image[enemy]->w, entities.image[enemy]->h);
		
		entities.gun[enemy].side = Gun::Side::CENTER;
		entities.gun[enemy].delay = (float)(rand() % 10) + 0.1;
		entities.gun[enemy].fire = true;
		entities.gun[enemy].direction = Vec2(0, 1);
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
	
	if(control::faster)
	{
		entities.velocity[i].y = -50;
	}
	else if(control::slower)
	{
		entities.velocity[i].y = -10;
	}
	else
	{
		entities.velocity[i].y = -20;
	}
	
	if(control::left)
	{
		entities.velocity[i].x = -50;
	}
	else if(control::right)
	{
		entities.velocity[i].x = 50;
	}
	else
	{
		entities.velocity[i].x = 0;
	}
	
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
	for(unsigned i = 0; i < entities.count(); i++)
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
				
				entities.velocity[bullet] = entities.velocity[i] + entities.gun[i].bullet_speed * entities.gun[i].direction;
				entities.image[bullet] = surface_from_string(bullet_pixels, screen->format);
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
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::bullet_mask) == ecs::bullet_mask)
		{
			entities.lifespan[i] -= dt;
		}
	}
}
void move_process(float dt)
{
	
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::move_mask) == ecs::move_mask)
		{
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void collision_damage_process(void)
{
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & ecs::collision_damage_mask) == ecs::collision_damage_mask)
		{
			Rect rectA(entities.position[i], entities.size[i]);
			for(unsigned j = i + 1; j < entities.count(); j++)
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
	for(unsigned i = 0; i < entities.count(); i++)
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
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	
	// draw stars (background)
	SDL_LockSurface(screen);
	for(auto i = stars.begin(); i != stars.end(); ++i)
	{
		int x = i->x - camera.x;
		int y = i->y - camera.y;
		if(x >= 0 && x < screen->w && y >= 0 && y < screen->h)
			((Uint32*)(screen->pixels))[y * screen->w + x] = SDL_MapRGB(screen->format, 128, 128, 128);
	}
	SDL_UnlockSurface(screen);
	
	// draw entities (space fighters and bullets)
	unsigned draw_mask = ecs::POSITION | ecs::IMAGE;
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & draw_mask) == draw_mask)
		{
			SDL_Rect position;
			position.x = entities.position[i].x - camera.x;
			position.y = entities.position[i].y - camera.y;
			SDL_BlitSurface(entities.image[i], NULL, screen, &position);
		}
	}
	
	SDL_Flip(screen);
}
void init_system(void)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO |
		SDL_INIT_EVENTTHREAD |
		SDL_INIT_TIMER);
	screen = SDL_SetVideoMode(
		window_width, window_height, 32, SDL_SWSURFACE);
}
void init_camera(void)
{
	camera.x = 0;
	camera.y = 0;
	camera.w = window_width;
	camera.h = window_height;
}


void cleanup_system(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD | SDL_INIT_TIMER);
	SDL_Quit();
}
int main(int argc, char ** argv)
{
	init_system();
	init_camera();
	
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
	cleanup_system();
	return 0;
}
