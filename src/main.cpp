#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iostream>
#include <SDL/SDL.h>
std::string const player_pixels = "\
        *        \n\
       * *       \n\
       * *       \n\
      * * *      \n\
     * * * *     \n\
   ** *   * **   \n\
**  * *   * *  **\n\
   **  ***  **   ";

std::string const bullet_pixels = "\
*\n\
*";

struct Vec2{
	float x, y;
	Vec2(void): x(0), y(0) {}
	Vec2(float X, float Y): x(X), y(Y) {}
};

Vec2 const operator+(Vec2 const & a, Vec2 const & b)
{
	Vec2 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}
Vec2 const & operator+=(Vec2 & a, Vec2 const & b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
Vec2 operator*(Vec2 const & a, float k)
{
	Vec2 b;
	b.x = a.x * k;
	b.y = a.y * k;
	return b;
}
Vec2 operator*(float k, Vec2 const & a)
{
	Vec2 b;
	b.x = a.x * k;
	b.y = a.y * k;
	return b;
}

struct Size{
	float w, h;
	Size(void): w(0), h(0) {}
	Size(float W, float H) : w(W), h(H) {}
};

// Rect represents a rectangle with top left corner as (x, y)
struct Rect{
	float x, y;
	float w, h;
	
	Rect(float X, float Y, float W, float H)
	{
		if(W < 0)
		{
			X -= W;
			W = -W;
		}
		if(H < 0)
		{
			Y -= H;
			H = -H;
		}
		x = X;
		y = Y;
		w = W;
		h = H;
	}
	Rect(void): x(0), y(0), w(0), h(0)
	{
	}
	Vec2 getCenter(void) const
	{
		Vec2 point;
		point.x = x + w / 2;
		point.y = y + h / 2;
		return point;
	}
	void setCenter(Vec2 const & center)
	{
		x = center.x - w / 2;
		y = center.y - h / 2;
	}
	float getCenterY(void) const
	{
		return y + w / 2;
	}
	void setCenterY(float center_y)
	{
		y = center_y - h / 2;
	}
	Vec2 getPosition(void) const
	{
		Vec2 point;
		point.x = x;
		point.y = y;
		return point;
	}
	float getTop(void) const
	{
		return y;
	}
	float getBottom(void) const
	{
		return y + w;
	}
	float getLeft(void) const
	{
		return x;
	}
	float getRight(void) const
	{
		return x + w;
	}
	bool contains(Vec2 const & p)const
	{
		return p.x >= x && p.x <= (x + w) &&
			p.y >= y && p.y <= (y + h);
	}
};
struct Gun{
	float wait_time;
	float delay;
	enum firing_side{
		CENTER,
		LEFT,
		RIGHT
	}side;
	bool fire;
	unsigned parent;
};
#define BIT(n) (1 << (n))
typedef uint16_t mask_t;
namespace component{
	enum flag{
		POSITION = BIT(0),
		VELOCITY = BIT(1),
		IMAGE = BIT(2),
		PLAYER_CONTROL = BIT(3),
		ENEMY_CONTROL = BIT(4),
		COLLISION_DAMAGE = BIT(5),
		COLLISION_EFFECT = BIT(6),
		LIFESPAN = BIT(7),
		GUN = BIT(8),
		SIZE = BIT(9)
	};
	mask_t const player_mask = POSITION | VELOCITY | SIZE | IMAGE | PLAYER_CONTROL | GUN;
	mask_t const bullet_mask = POSITION | VELOCITY | SIZE | IMAGE | LIFESPAN;
	mask_t const move_mask = POSITION | VELOCITY;
	mask_t const shooter_mask = POSITION | SIZE | GUN;
}
template <unsigned MAX_ENTITIES>
struct Entity{
	mask_t mask[MAX_ENTITIES];
	Vec2 position[MAX_ENTITIES];
	Vec2 velocity[MAX_ENTITIES];
	Size size[MAX_ENTITIES];
	SDL_Surface * image[MAX_ENTITIES];
	int collision_damage[MAX_ENTITIES];
	float lifespan[MAX_ENTITIES];
	Gun gun[MAX_ENTITIES];
	
	Entity(void): m_count(0)
	{
		for(unsigned i = 0; i < MAX_ENTITIES; i++)
			mask[i] = 0;
	}
	unsigned claim(void)
	{
		if(holes > 0)
		{
			for(unsigned i = 0; i < m_count; i++)
			{
				if(mask[i] == 0)
				{
					holes--;
					return i;
				}
			}
		}
		if(m_count == MAX_ENTITIES)
			return m_count - 1;
		else
			return m_count++;
	}
	void remove(unsigned i)
	{
		mask[i] = 0;
		SDL_FreeSurface(image[i]);
		image[i] = NULL;
		holes++;
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
	unsigned holes;
};


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
	
	SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width, height, 32,
		format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if(surface == NULL)
		return NULL;
	Uint32 * data = (Uint32*)(surface->pixels);
	
	SDL_LockSurface(surface);
	unsigned i = 0;
	for(auto iter = pixels.begin(); i < width * height && iter < pixels.end(); ++iter)
	{
		if(*iter != '\n')
		{
			if(*iter == '*')
				data[i] = 0xffffffff;
			else
				data[i] = 0x00000000;
			i++;
		}
	}
	SDL_UnlockSurface(surface);
	
	return surface;
}
bool running = true;
float const dt_unit = 1.0 /1000.0;
unsigned window_width = 320;
unsigned window_height = 320;

SDL_Surface * screen = NULL;
Rect camera;

unsigned player;

Entity<1000> entities;
std::deque<Vec2> stars;

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
void player_control(void)
{
	if(control::faster)
	{
		entities.velocity[player].y = -50;
	}
	else if(control::slower)
	{
		entities.velocity[player].y = -10;
	}
	else
	{
		entities.velocity[player].y = -20;
	}
	
	if(control::left)
	{
		entities.velocity[player].x = -50;
	}
	else if(control::right)
	{
		entities.velocity[player].x = 50;
	}
	else
	{
		entities.velocity[player].x = 0;
	}
	
	if(control::fire)
	{
		entities.gun[player].fire = true;
	}
	else
	{
		entities.gun[player].fire = false;
	}
}
void shooter_process(float dt)
{
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & component::shooter_mask) == component::shooter_mask)
		{
			if(entities.gun[i].wait_time > 0)
				entities.gun[i].wait_time -= dt;
			if(entities.gun[i].fire && entities.gun[i].wait_time <= 0)
			{
				entities.gun[i].wait_time += entities.gun[i].delay;
				
				unsigned bullet = entities.claim();
				entities.mask[bullet] = component::bullet_mask;
				
				unsigned parent = entities.gun[i].parent;
				if(entities.gun[i].side == Gun::LEFT)
				{
					entities.position[bullet] = entities.position[parent];
					entities.gun[i].side = Gun::RIGHT;
				}
				else if(entities.gun[i].side == Gun::RIGHT)
				{
					entities.position[bullet] = entities.position[parent] + Vec2(entities.size[parent].w, 0);
					entities.gun[i].side = Gun::LEFT;
				}
				else
				{
					entities.position[bullet] = entities.position[parent] + Vec2(entities.size[parent].w / 2, 0);
				}
				
				entities.velocity[bullet] = entities.velocity[player] + Vec2(0, -50);
				entities.image[bullet] = surface_from_string(bullet_pixels, screen->format);
				entities.lifespan[bullet] = 5.0;
			}
		}
	}
}
void bullet_process(float dt)
{
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & component::bullet_mask) == component::bullet_mask)
		{
			entities.lifespan[i] -= dt;
			if(entities.lifespan[i] < 0)
			{
				entities.remove(i);
			}
		}
	}
}
void move(float dt)
{
	
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if((entities.mask[i] & component::move_mask) == component::move_mask)
		{
			entities.position[i] += dt * entities.velocity[i];
		}
	}
}
void update(float const dt_unit)
{
	player_control();
	shooter_process(dt_unit);
	bullet_process(dt_unit);
	move(dt_unit);
	camera.setCenterY(floor(entities.position[player].y));
	
	if((rand() % 1000) < 10)
	{
		Vec2 star;
		star.y = entities.position[player].y - window_height / 2;
		star.x = rand() % window_width;
		stars.push_back(star);
	}
	while(stars.front().y > camera.getBottom())
	{
		stars.pop_front();
	}
}
void draw(void)
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	
	// draw stars (background)
	SDL_LockSurface(screen);
	for(auto i = stars.begin(), end = stars.end(); i < end; ++i)
	{
		unsigned x = i->x - camera.x;
		unsigned y = i->y - camera.y;
		if(x >= 0 && x < screen->w && y >= 0 && y < screen->h)
			((Uint32*)(screen->pixels))[y * screen->w + x] = SDL_MapRGB(screen->format, 128, 128, 128);
	}
	SDL_UnlockSurface(screen);
	
	// draw entities (space fighters)
	unsigned draw_mask = component::POSITION | component::IMAGE;
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

void init_player(void)
{
	player = entities.claim();
	
	entities.mask[player] = component::player_mask;
	
	entities.image[player] = surface_from_string(player_pixels, screen->format);
	
	Rect player_rect(0, 0, entities.image[player]->w, entities.image[player]->h);
	player_rect.setCenter(camera.getCenter());
	entities.position[player] = player_rect.getPosition();
	
	entities.size[player] = Size(entities.image[player]->w, entities.image[player]->h);
	
	entities.gun[player].parent = player;
	entities.gun[player].side = Gun::LEFT;
	entities.gun[player].delay = 0.1;
}
void cleanup_system(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO |
		SDL_INIT_EVENTTHREAD |
		SDL_INIT_TIMER);
	SDL_Quit();
}
int main(int argc, char ** argv)
{
	init_system();
	init_camera();
	
	init_player();
	
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
	}
	cleanup_system();
	return 0;
}
