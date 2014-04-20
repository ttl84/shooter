#include "font.h"
#include "filereader/FileReader.h"
#include <fstream>
unsigned Font::getWidth() const
{
	for(auto pair : *this)
	{
		return pair.second.getWidth();
	}
	return 0;
}
unsigned Font::getHeight() const
{
	for(auto pair : *this)
	{
		return pair.second.getHeight();
	}
	return 0;
}
static std::tuple<Image, bool> loadSymbol(StringObject * obj)
{
	if(obj == nullptr)
		return std::make_tuple(Image(), false);
	CharImg ci(obj->datum);
	Palette palette;
	palette.emplace('#', Pixel{255, 255, 255, 255});
	Pixel colorkey{0, 0, 0};
	return std::make_tuple(Image(ci, palette, colorkey), true);
}
std::tuple<Font, bool> loadFont(std::string filename)
{
	Font font;
	std::ifstream fileStream(filename);
	if(not fileStream)
	{
		std::cerr << "unable to open font file " << filename << std::endl;
		return std::make_tuple(font, false);
	}
	
	FileReader reader(fileStream);
	
	
	for(char c : std::string("0123456789" "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
	{
		std::string name("_");
		name += c;
		StringObject * bytesObject = reader.getString(name);
		
		Image img;
		bool good;
		std::tie(img, good) = loadSymbol(bytesObject);
		if(good)
			font.emplace(c, img);
	}
	return std::make_tuple(font, true);
}