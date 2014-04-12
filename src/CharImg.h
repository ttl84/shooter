#ifndef CharImg_H
#define CharImg_H
#include <string>
class CharImg{
	std::string myChars;
	unsigned myWidth, myHeight;
	
public:
	CharImg(void) : myChars(""), myWidth(0), myHeight(0) {}
	CharImg(std::string const & s, char delimiter = '\n');
	unsigned width(void) const
	{
		return myWidth;
	}
	unsigned height(void) const
	{
		return myHeight;
	}
	std::string const & chars(void) const
	{
		return myChars;
	}
	char at(unsigned row, unsigned column) const
	{
		return myChars[row * myWidth + column];
	}

	int compare(CharImg const & other) const
	{
		if(myWidth < other.width())
			return -1;
		else if(myWidth > other.width())
			return 1;
		else if(myHeight < other.height())
			return -2;
		else if(myHeight > other.height())
			return 2;
		else if(myChars < other.chars())
			return -3;
		else if(myChars > other.chars())
			return 3;
		else
			return 0;
	}
	bool operator < (CharImg const & other) const
	{
		return compare(other) < 0;
	}
	bool operator = (CharImg const & other) const
	{
		return compare(other) == 0;
	}

};
#endif
