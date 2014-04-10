#ifndef Object_h
#define Object_h
#include <string>
#include <vector>
struct Object {
	enum class Type {
		INTEGER, REAL, BOOLEAN, STRING, LIST
	} type;
	int count;
	union {
		long integer;
		double real;
		bool boolean;
		std::string * string;
		std::vector<Object *> * list;
	} datum;
};
#endif
