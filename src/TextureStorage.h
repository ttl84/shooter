#ifndef TextureStorage_H
#define TextureStorage_H
#include <unordered_map>
#include <map>
#include <string>
#include <SDL2/SDL.h>
#include "CharImg.h"
struct Pixel{
	uint8_t r, g, b, a;
};
class TextureStorage{
	std::map<CharImg, SDL_Texture *> cache;
	SDL_Renderer * renderer;
public:
	TextureStorage(void) : renderer(NULL) {}
	TextureStorage(SDL_Renderer * R): renderer(R) {}
	SDL_Texture * load(CharImg const & ci, std::map<char, Pixel> const & pixelmap, Pixel colorkey);
};
#endif
