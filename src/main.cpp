#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_Image.h>
char const * player_img = "\
        *        \
       * *       \
       * *       \
      *   *      \
     *     *     \
   **       **   \
**             **\
   ***     ***   ";
bool running = true;
float const dt = 0.1;
unsigned window_width = 320;
unsigned window_height = 320;

SDL_Surface * screen = NULL;
SDL_Rect camera;

unsigned const player = 0;

namespace control{
	bool faster = false;
	bool slower = false;
	bool left = false;
	bool right = false;
	bool fire = false;
}
struct vec2{
	unsigned x, y;
};

#define MAX_ENTITIES 200
#define BIT(n) (1 << (n))
namespace component{
	enum flag{
		POSITION = BIT(0),
		VELOCITY = BIT(1),
		IMAGE = BIT(2)
	};
}
struct entities{
	unsigned component_flag[MAX_ENTITIES];
	vec2 position[MAX_ENTITIES];
	vec2 velocity[MAX_ENTITIES];
	SDL_Surface * image[MAX_ENTITIES];
};
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
		}
		else if(e.type == SDL_QUIT)
		{
			running = false;
		}
	}
}
void update(void)
{
	
}
void draw(void)
{
	SDL_FillRect(screen, NULL, 0x00000000);
	SDL_Flip(screen);
}
int main(int argc, char ** argv)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO |
		SDL_INIT_EVENTTHREAD |
		SDL_INIT_TIMER);
	screen = SDL_SetVideoMode(
		window_width, window_height, 32, SDL_SWSURFACE);
	camera.x = 0;
	camera.y = 0;
	camera.w = window_width;
	camera.h = window_height;
	
	while(running)
	{
		handle_event();
		update();
		draw();
	}
	SDL_QuitSubSystem(SDL_INIT_VIDEO |
		SDL_INIT_EVENTTHREAD |
		SDL_INIT_TIMER);
	SDL_Quit();
	return 0;
}
