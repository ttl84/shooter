#ifndef CharImg_H
#define CharImg_H
#include <string>
#include <tuple>
class CharImg{
	std::string myChars;
	unsigned myWidth, myHeight;
	
public:
	CharImg() : myChars(""), myWidth(0), myHeight(0) {}
	CharImg(std::string const & s, char delimiter = '\n');
	unsigned getWidth() const
	{
		return myWidth;
	}
	unsigned getHeight() const
	{
		return myHeight;
	}
	std::string chars() const
	{
		return myChars;
	}
	char at(unsigned row, unsigned column) const
	{
		return myChars[row * myWidth + column];
	}


	bool operator < (CharImg const & other) const
	{
		return std::tie(myWidth, myHeight, myChars) <
			std::tie(other.myWidth, other.myHeight, other.myChars);
	}

};
#endif
