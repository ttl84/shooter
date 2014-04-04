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
	std::string getString(std::string key) const;
	long getInteger(std::string key) const;
	double getReal(std::string key) const;
	bool getBoolean(std::string key) const;
};
#endif
