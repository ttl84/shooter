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
	
	bool isType(Type type)
	{
		return type == getType();
	}
	virtual ~Object(){};
	virtual Type getType() const = 0;
};
struct IntegerObject : public Object{
	static constexpr Type typeCode = Object::Type::INTEGER;
	long datum;
	Type getType() const
	{
		return typeCode;
	}
};
struct RealObject : public Object{
	static constexpr Type typeCode = Object::Type::REAL;
	double datum;
	Type getType() const
	{
		return typeCode;
	}
};
struct BooleanObject : public Object{
	static constexpr Type typeCode = Object::Type::BOOLEAN;
	bool datum;
	Type getType() const
	{
		return typeCode;
	}
};
struct StringObject : public Object{
	static constexpr Type typeCode = Object::Type::STRING;
	std::string datum;
	Type getType() const
	{
		return typeCode;
	}
};
struct ListObject : public Object{
	static constexpr Type typeCode = Object::Type::LIST;
	std::vector<Object *> datum;
	Type getType() const
	{
		return typeCode;
	}
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
