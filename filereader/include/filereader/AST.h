#ifndef AST_h
#define AST_h
#include <vector>
#include <memory>
#include "Token.h"
struct AST final{
	enum class Type{
		TOP, ASSIGNMENT, 
		IDENTIFIER, REAL, INTEGER, BOOLEAN, STRING, 
		LIST, 
		ERROR
	} type;
	Token start;
	
	// children and parent
	void append(AST * child);
	AST * getParent(void);
	std::unique_ptr<AST> & child(unsigned i);
	
	
	
	// constructors
	AST(Type nodeType, Token nodeStart)
		:type(nodeType), start(nodeStart),parent(nullptr){}
private:
	AST * parent;
	std::vector<std::unique_ptr<AST>> children;

public:
	// iterate through children
	auto begin(void) -> decltype(children.begin())
	{
		return children.begin();
	}
	auto end(void) -> decltype(children.begin())
	{
		return children.end();
	}
	
};

#endif
