#ifndef FileReader_H
#define FileReader_H
#include <memory>
#include <map>
#include <set>
#include <string>
#include <istream>

#include "Object.h"
class FileReader{
private:
	std::map<std::string, Object *> data;
	std::vector<std::unique_ptr<Object>> pool;
	Object * get(std::string key);
public:
	FileReader(std::istream & is);
	
	StringObject * getString(std::string key);
	IntegerObject * getInteger(std::string key);
	RealObject * getReal(std::string key);
	BooleanObject * getBoolean(std::string key);
	ListObject * getList(std::string key);
};
#endif
