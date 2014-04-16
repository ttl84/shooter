#ifndef Object_h
#define Object_h
#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct Object {
	enum class Type :unsigned{
		INTEGER, REAL, BOOLEAN, STRING, LIST
	};
protected:
	Type const type;
	Object(Type t) : type(t) {}
	
public:
	virtual ~Object(){};
	Type getType() const
	{
		return type;
	}
};
struct IntegerObject : public Object{
	static constexpr Type typeCode = Object::Type::INTEGER;
	long datum;
	IntegerObject() : Object(typeCode){}
};
struct RealObject : public Object{
	static constexpr Type typeCode = Object::Type::REAL;
	double datum;
	RealObject() : Object(typeCode){}
};
struct BooleanObject : public Object{
	static constexpr Type typeCode = Object::Type::BOOLEAN;
	bool datum;
	BooleanObject() : Object(typeCode){}
};
struct StringObject : public Object{
	static constexpr Type typeCode = Object::Type::STRING;
	std::string datum;
	StringObject() : Object(typeCode){}
};
struct ListObject : public Object{
	static constexpr Type typeCode = Object::Type::LIST;
	std::vector<Object *> datum;
	ListObject() : Object(typeCode){}
};
template<class T>
T * downcast(Object * obj)
{
	if(obj != nullptr and obj->getType() == T::typeCode)
		return static_cast<T*>(obj);
	else
		return nullptr;
}
#endif
