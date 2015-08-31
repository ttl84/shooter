#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <unordered_map>
#include "SDL2/SDL.h"
#include "math/Rect.h"
class Window{
	// only Video has the privilege of constructing a Window.
	friend class Video;

	std::string title;
	unsigned width;
	unsigned height;

	SDL_Renderer * renderer;
	SDL_Window * window;

	std::unordered_map<char, SDL_Texture*> fontTextureMap;
	std::unordered_map<std::string, SDL_Texture*> textureMap;

	Window();
	Window(char const * title, unsigned w, unsigned h);
public:
	~Window();

	// return true if the window is ready for use
	bool good() const;

	// free up all resources
	void destroy();

	// load a font, and return true for success
	bool loadFont(char const * path);

	// load a texture from image file
	SDL_Texture* loadTexture(char const * path);
};
#endif // WINDOW_H

