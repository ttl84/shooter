#ifndef FileReader_H
#define FileReader_H
#include <map>
#include <string>
#include <istream>
#include "TokenStream.h"
class FileReader{
private:
	std::map<std::string, Token> data;
public:
	FileReader(std::istream & is);
	bool getString(std::string key, std::string & output) const;
	bool getInteger(std::string key, long & output) const;
	bool getReal(std::string key, double & output) const;
	bool getBoolean(std::string key, bool & output) const;
};
#endif
