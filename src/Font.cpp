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
static void loadSymbol(Font & font, Object * obj)
{
	auto listObj = downcast<ListObject>(obj);
	if(listObj == nullptr or listObj->datum.size() != 2)
		return;

	auto strObj0 = downcast<StringObject>(listObj->datum[0]);
	auto strObj1 = downcast<StringObject>(listObj->datum[1]);
	if(strObj0 == nullptr or strObj1 == nullptr)
		return;

	// load the pixel layout
	CharImg ci(strObj1->datum);

	// make the palette
	Palette palette;
	palette.emplace('#', Pixel{255, 255, 255, 255});

	// colorkey
	Pixel colorkey{0, 0, 0};

	// finally make the image
	Image img(ci, palette, colorkey);

	// then insert into font
	font.emplace(strObj0->datum[0], img);
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

	auto tableObject = reader.getList("table");
	if(tableObject == nullptr)
		return std::make_tuple(font, false);
	
	for(auto obj : tableObject->datum)
	{
		loadSymbol(font, obj);
	}
	return std::make_tuple(font, true);
}