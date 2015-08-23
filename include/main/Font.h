#ifndef font_H
#define font_H
#include "image/Image.h"
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
