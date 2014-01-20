#include "TextureStorage.h"
SDL_Surface * surface_from_string(std::string const & pixels, std::map<char, Uint32> const & pixelmap, Uint32 colorkey, char delim)
{
	unsigned width = 0;
	for(unsigned i = 0; i < pixels.length(); i++)
	{
		if(pixels[i] == delim)
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
	
	SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_SetColorKey(surface, SDL_TRUE, colorkey);
	
	if(surface == NULL)
		return NULL;
	Uint32 * data = (Uint32*)(surface->pixels);
	
	SDL_LockSurface(surface);
	unsigned i = 0;
	for(auto iter = pixels.begin(); i < width * height && iter != pixels.end(); ++iter)
	{
		if(*iter != delim)
		{
			if(pixelmap.find(*iter) == pixelmap.end())
				data[i] = colorkey;
			else
				data[i] = pixelmap.at(*iter);
			i++;
		}
	}
	SDL_UnlockSurface(surface);
	return surface;
}
SDL_Texture * TextureStorage::load(std::string const & pixels, std::map<char, Uint32> const & pixelmap, Uint32 colorkey, char delim)
{
	if(renderer == NULL)
		return NULL;
	SDL_Texture * texture = NULL;
	if(cache.find(pixels) == cache.end())
	{
		SDL_Surface * surface = surface_from_string(pixels, pixelmap, colorkey, delim);
		if(surface != NULL)
		{
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
		}
		if(texture != NULL)
			cache[pixels] = texture;
	}
	else
		texture = cache[pixels];
	return texture;
}
SDL_Texture * TextureStorage::load(std::string const & pixels)
{
	SDL_Surface * surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
	std::map<char, Uint32> pixelmap{{'*',SDL_MapRGB(surface->format, 255, 255, 255)}};
	
	SDL_Texture * texture = load(pixels, pixelmap,
		SDL_MapRGB(surface->format, 255, 0, 0), '\n');
	SDL_FreeSurface(surface);
	return texture;
}