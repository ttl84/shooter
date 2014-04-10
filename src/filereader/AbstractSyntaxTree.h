#ifndef AbstractSyntaxTree_h
#define AbstractSyntaxTree_h
#include <vector>
#include <memory>
struct AST{
	enum class Type{
		TOP, ASSIGNMENT, 
		IDENTIFIER, REAL, INTEGER, BOOLEAN, STRING, 
		LIST, 
		ERROR, NONE
	} type;
	Token start;
	
	void append(AST * child);
	AST * getParent(void);
	std::unique_ptr<AST> & at(unsigned i);
	
	// iterator functions
	typedef std::vector<std::unique_ptr<AST>>::iterator iterator;
	friend AST::iterator begin(AST const &);
	friend AST::iterator end(AST const &);
private:
	AST * parent;
	std::vector<std::unique_ptr<AST>> children;
	
};

#endif
