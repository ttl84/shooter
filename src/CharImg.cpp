#include "CharImg.h"
#include <vector>
#include <string>
std::vector<std::string> tokenize(std::string const & s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	for(char c : s)
	{
		if(c == delimiter)
		{
			tokens.push_back(token);
			token = "";
		}
		else
		{
			token += c;
		}
	}
	tokens.push_back(token);
	return tokens;
}
std::string & extend(std::string & s, unsigned length, char c)
{
	while(s.length() < length)
		s += c;
	return s;
}
CharImg::CharImg(std::string const & s, char delimiter)
{
	std::vector<std::string> tokens = tokenize(s, delimiter);
	myWidth = 0;
	myHeight = 0;
	for(std::string& t : tokens)
	{
		if(t.length() > myWidth)
			myWidth = t.length();
	}
	myChars = "";
	for(std::string& t : tokens)
	{
		myChars += extend(t, myWidth, ' ');
		myHeight++;
	}
}