#ifndef FileReader_H
#define FileReader_H
#include <map>
#include <string>
#include <istream>
#include "TokenStream.h"
#include "Object.h"
class FileReader{
private:
	std::map<std::string, Object*> data;
	Object * get(std::string key);
	Object * getForType(std::string key, Object::Type type);
public:
	FileReader(std::istream & is);
	
	Object * getString(std::string key);
	Object * getInteger(std::string key);
	Object * getReal(std::string key);
	Object * getBoolean(std::string key);
};
#endif
