#ifndef FileReader_H
#define FileReader_H
#include <memory>
#include <map>
#include <string>
#include <istream>

#include "Object.h"
class FileReader{
private:
	std::map<std::string, std::shared_ptr<Object>> data;
	std::shared_ptr<Object> get(std::string key);
	std::shared_ptr<Object> getForType(std::string key, Object::Type type);
public:
	FileReader(std::istream & is);
	
	std::shared_ptr<Object> getString(std::string key);
	std::shared_ptr<Object> getInteger(std::string key);
	std::shared_ptr<Object> getReal(std::string key);
	std::shared_ptr<Object> getBoolean(std::string key);
	std::shared_ptr<Object> getList(std::string key);
};
#endif
