#ifndef Object_h
#define Object_h
#include <string>
#include <vector>
#include <memory>
struct Object {
	enum class Type {
		INTEGER, REAL, BOOLEAN, STRING, LIST
	} const type;
	
	union {
		long integer;
		double real;
		bool boolean;
		std::string * string;
		std::vector<std::shared_ptr<Object>> * list;
	} datum;

	Object(Type init) : type(init)
	{
	}
	~Object()
	{
		if(type == Type::STRING)
		{
			delete datum.string;
			datum.string = nullptr;
		}
		else if(type == Type::LIST)
		{
			delete datum.list;
			datum.list = nullptr;
		}
	}
	static std::shared_ptr<Object> makeInteger()
	{
		return std::make_shared<Object>(Type::INTEGER);
	}
	static std::shared_ptr<Object> makeReal()
	{
		return std::make_shared<Object>(Type::REAL);
	}
	static std::shared_ptr<Object> makeBoolean()
	{
		return std::make_shared<Object>(Type::BOOLEAN);
	}
	static std::shared_ptr<Object> makeString()
	{
		auto obj = std::make_shared<Object>(Type::STRING);
		obj->datum.string = new std::string;
		return obj;
	}
	static std::shared_ptr<Object> makeList()
	{
		auto obj = std::make_shared<Object>(Type::LIST);
		obj->datum.list = new std::vector<std::shared_ptr<Object>>;
		return obj;
	}
};
#endif
