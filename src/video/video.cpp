#include "video.h"
#include "SDL2/SDL_image.h"
#include <vector>
#include <iostream>

Video::Video()
{
	int result1 = SDL_InitSubsystem(SDL_INIT_VIDEO);
	if(result1 != 0) {
		good = false;
		std::cout << "sdl video error: " << SDL_GetError() << "\n";
		return;
	}

	int flags = IMG_INIT_PNG;
	int result2 = IMG_Init(flags);
	if(result2 != flags) {
		good = false;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		std::cout << "sdl_image error: " << IMG_Error() << "\n";
		return;
	}
	good = true;
}

Video::~Video()
{
	if(good) {
		windows.clear();
		IMG_Quit();
		SDL_QuitSubsystem(SDL_INIT_VIDEO);
	}
}

Window*
Video::makeWindow(const char *name, unsigned width, unsigned height)
{
	for(auto & win : windows) {
		if(!win.good()) {
			win = Window(name, width, height);
			if(win.good())
				return &win;
			else
				return nullptr;
		}
	}
	windows.emplace_back(name, width, height);
	if(windows.back().good())
		return &windows.back();
	else
		return nullptr;
}

