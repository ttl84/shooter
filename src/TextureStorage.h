#ifndef TextureStorage_H
#define TextureStorage_H
#include <map>
#include <string>

#include <SDL2/SDL.h>
class TextureStorage{
	std::map<std::string, SDL_Texture *> cache;
	SDL_Renderer * renderer;
public:
	TextureStorage(void) : renderer(NULL) {}
	TextureStorage(SDL_Renderer * R): renderer(R) {}
	SDL_Texture * load(std::string const & pixels);
	SDL_Texture * load(std::string const & pixels, std::map<char, Uint32> const & pixelmap, Uint32 colorkey, char delim);
};
#endif
