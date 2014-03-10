#ifndef Image_h
#define Image_h
#include "CharImg.h"

#include "SDL2/SDL.h"
#include <map>

struct Pixel{
	uint8_t r, g, b, a;
};
typedef std::map<uint8_t, Pixel> Palette;
class Image{
	CharImg myBytes;
	Palette myPalette;
	Pixel myColorkey;
	SDL_Texture * myTexture;
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
	Image(CharImg const & bytes, Palette const & palette, Pixel colorkey)
	: myBytes{bytes}, myPalette{palette}, myColorkey(colorkey), myTexture{nullptr}
	{
	}
	SDL_Texture * getTexture(SDL_Renderer *);
};
#endif
