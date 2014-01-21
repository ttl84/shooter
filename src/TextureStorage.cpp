#include "TextureStorage.h"
#include "CharImg.h"
#include <iostream>
SDL_Surface * surface_from_string(CharImg const & ci, std::map<char, Pixel> const & pixelmap, Pixel colorkey)
{
	unsigned width = ci.width();
	unsigned height = ci.height();
	SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, colorkey.r, colorkey.g, colorkey.b));
	
	if(surface == NULL)
		return NULL;
	Uint32 * data = (Uint32*)(surface->pixels);
	
	SDL_LockSurface(surface);
	unsigned i = 0;
	for(char c : ci.chars())
	{
		Pixel pix = (pixelmap.find(c) == pixelmap.end() ? colorkey : pixelmap.at(c));
		data[i] = SDL_MapRGB(surface->format, pix.r, pix.g, pix.b);
		i++;
	}
	SDL_UnlockSurface(surface);
	return surface;
}
SDL_Texture * TextureStorage::load(CharImg const & ci, std::map<char, Pixel> const & pixelmap, Pixel colorkey)
{
	if(renderer == NULL)
		return NULL;
	SDL_Texture * texture = NULL;
	if(cache.find(ci) == cache.end())
	{
		std::cout << "loading new texture of width " << ci.width() << " and height " << ci.height() << '\n';
		SDL_Surface * surface = surface_from_string(ci, pixelmap, colorkey);
		if(surface != NULL)
		{
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
		}
		if(texture != NULL)
			cache.insert({ci, texture});
	}
	else
		texture = cache.at(ci);
	return texture;
}