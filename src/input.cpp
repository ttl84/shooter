#include "input.h"
#include <SDL2/SDL.h>

namespace control{
	bool faster = false;
	bool slower = false;
	bool left = false;
	bool right = false;
	bool fire = false;
	bool quit = false;
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
				control::quit = true;
				
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
			control::quit = true;
		}
	}
}
