#include "Image.h"
#include "debug.h"
static SDL_Surface * makeSurface(Image const & img)
{
	
	CharImg const & bytes = img.getBytes();
	Palette const & palette = img.getPalette();
	Pixel colorkey = img.getColorkey();
	unsigned width = bytes.width();
	unsigned height = bytes.height();
	
	SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, colorkey.r, colorkey.g, colorkey.b));
	
	if(surface == NULL)
		return NULL;
	Uint32 * data = (Uint32*)(surface->pixels);
	
	SDL_LockSurface(surface);
	unsigned i = 0;
	for(char c : bytes.chars())
	{
		Pixel pix = (palette.find(c) == palette.end() ? colorkey : palette.at(c));
		data[i] = SDL_MapRGB(surface->format, pix.r, pix.g, pix.b);
		i++;
		
	}
	SDL_UnlockSurface(surface);
	return surface;
}
SDL_Texture * Image::makeTexture(SDL_Renderer * renderer)
{
	if(not good)
	{
		debug::err << "image is not initialized" << std::endl;
		return nullptr;
	}
	SDL_Texture * texture = NULL;
	SDL_Surface * surface = makeSurface(*this);
	if(surface != NULL)
	{
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		
	}
	else
		debug::err << "unable to make surface" << std::endl;
	return texture;
}
