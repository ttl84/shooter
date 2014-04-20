#ifndef Image_h
#define Image_h
#include "CharImg.h"

#include "SDL2/SDL.h"
#include <map>
#include <iostream>
struct Pixel{
	uint8_t r, g, b, a;
};
typedef std::map<uint8_t, Pixel> Palette;
class Image{
	CharImg myBytes;
	Palette myPalette;
	Pixel myColorkey;
	bool good;
public:
	CharImg const & getBytes() const
	{
		return myBytes;
	}
	Palette const & getPalette(void) const
	{
		return myPalette;
	}
	Pixel getColorkey(void) const
	{
		return myColorkey;
	}
	unsigned getWidth() const
	{
		return myBytes.getWidth();
	}
	unsigned getHeight() const
	{
		return myBytes.getHeight();
	}
	Image(CharImg const & bytes, Palette const & palette, Pixel colorkey)
	: myBytes(bytes), myPalette(palette), myColorkey(colorkey), good(true)
	{
		
	}
	Image() : good(false){}
	SDL_Texture * makeTexture(SDL_Renderer *);
};
#endif
