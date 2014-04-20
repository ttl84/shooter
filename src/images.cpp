#include "images.h"
#include "filereader/FileReader.h"
#include "debug.h"
#include <fstream>
static std::string makeImagePath(std::string name)
{
	std::string dir("./images/");
	std::string extension(".txt");
	return dir + name + extension;
}
static std::tuple<uint8_t, Pixel, bool>
interpretColourTuple(Object * src)
{
	if(src->getType() == Object::Type::LIST)
	{
		auto & colourTuple = downcast<ListObject>(src)->datum;
		if(colourTuple.size() == 4)
		{
			StringObject * symbolObject = downcast<StringObject>(colourTuple.at(0));
			IntegerObject * redObject = downcast<IntegerObject>(colourTuple.at(1));
			IntegerObject * greenObject = downcast<IntegerObject>(colourTuple.at(2));
			IntegerObject * blueObject = downcast<IntegerObject>(colourTuple.at(3));
			
			if(symbolObject != nullptr and symbolObject->datum.length() == 1 and
				redObject != nullptr and
				greenObject != nullptr and
				blueObject != nullptr)
			{
				uint8_t symbol = symbolObject->datum[0];
				Pixel colour;
				colour.r = redObject->datum;
				colour.g = greenObject->datum;
				colour.b = blueObject->datum;
				colour.a = 255;
				return std::make_tuple(symbol, colour, true);
			}
		}
		
	}
	return std::make_tuple(0, Pixel{0}, false);
}
static std::tuple<Palette, bool> interpretPalette(ListObject * src)
{
	Palette dst;

	auto & colourObjectList = src->datum;
	for(auto & colourObject : colourObjectList)
	{
		uint8_t symbol;
		Pixel pixel;
		bool good;
		std::tie(symbol, pixel, good) = interpretColourTuple(colourObject);
		if(good)
		{
			dst[symbol]= pixel;
		}
	}
	return std::make_tuple(dst, true);
}
static Pixel genColourkey(Palette & pal)
{
	for(unsigned r = 0; r < 256; r++)
	{
		for(unsigned g = 0; g < 256; g++)
		{
			for(unsigned b = 0; b < 256; b++)
			{
				Pixel colour{uint8_t(r), uint8_t(g), uint8_t(b)};
				bool found = false;
				for(auto pair : pal)
				{
					auto other = pair.second;
					if(std::tie(colour.r, colour.g, colour.b) ==
						std::tie(other.r, other.g, other.b))
					{
						found = true;
						break;
					}
				}
				if(not found)
					return colour;
			}
		}
	}
	return Pixel{0};
}
std::tuple<Image, bool> loadImage(std::string name)
{
	std::string path = makeImagePath(name);
	std::ifstream fileStream(path);
	if(not fileStream)
	{
		debug::err << "unable to open file [" << path << "]\n";
		return std::make_tuple(Image(), false);
	}
	FileReader reader(fileStream);
	

	StringObject * bytesObject = reader.getString("bytes");
	if(bytesObject == nullptr)
	{
		debug::err << "image file [" << path << "is missing object [bytes]\n";
		return std::make_tuple(Image(), false);
	}
	
	ListObject * paletteObject = reader.getList("palette");
	if(paletteObject == nullptr)
	{
		debug::err << "image file [" << path << "is missing object [palette]\n";
		return std::make_tuple(Image(), false);
	}
	
	
	CharImg bytes(bytesObject->datum);
	
	Palette palette;
	bool goodPalette;
	std::tie(palette, goodPalette) = interpretPalette(paletteObject);
	if(not goodPalette)
		debug::err << "unable to load palette\n";
	
	Pixel colourkey = genColourkey(palette);
	
	Image image(bytes, palette, colourkey);
	return std::make_tuple(image, true);
}