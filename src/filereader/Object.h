#ifndef Object_h
#define Object_h
#include <string>
#include <vector>
struct Object {
	enum class Type {
		INTEGER, REAL, BOOLEAN, STRING, LIST
	} const type;
	
	union {
		long integer;
		double real;
		bool boolean;
		std::string * string;
		std::vector<Object *> * list;
	} datum;
	
private:
	Object(Type init) : type(init){}
	~Object()
	{
		if(type == Type::STRING)
			delete datum.string;
		else if(type == Type::LIST)
			delete datum.list;
	}
public:
	static Object * makeInteger()
	{
		return new Object(Type::INTEGER);
	}
	static Object * makeReal()
	{
		return new Object(Type::REAL);
	}
	static Object * makeBoolean()
	{
		return new Object(Type::BOOLEAN);
	}
	static Object * makeString()
	{
		auto obj = new Object(Type::STRING);
		obj->datum.string = new std::string;
		return obj;
	}
	static Object * makeList()
	{
		auto obj = new Object(Type::LIST);
		obj->datum.list = new std::vector<Object *>;
		return obj;
	}
	static void destroy(Object * obj)
	{
		delete obj;
	}
};
#endif
