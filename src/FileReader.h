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
};
#endif
