#ifndef FONT_H
#define FONT_H
#include "video/Image.h"
#include <string>
#include <tuple>
#include <map>
class Font : public std::map<char, Image>{
public:
	unsigned getWidth() const;
	unsigned getHeight() const;
};
std::tuple<Font, bool> loadFont(std::string filename);

#endif
