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
	unsigned count;
public:
	Object(Type init) : type(init), count(1){}
	static Object * inc(Object * obj)
	{
		obj->count++;
		return obj;
	}
	static Object * dec(Object * obj)
	{
		obj->count--;
		if(obj->count == 0)
		{
			if(obj->type == Type::STRING)
			{
				delete obj->datum.string;
			}
			else if(obj->type == Type::LIST)
			{
				for(auto child : *(obj->datum.list))
					dec(child);
				delete obj->datum.list;
			}
			delete obj;
			obj = nullptr;
		}
		return obj;
	}
};
#endif
