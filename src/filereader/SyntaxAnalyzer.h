#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include <vector>
#include <memory>
#include "TokenStream.h"
struct AST{
	enum class Type{
		TOP, ASSIGNMENT, 
		IDENTIFIER, REAL, INTEGER, BOOLEAN, STRING, 
		LIST, 
		ERROR, NONE
	} type;
	Token start;
	std::vector<std::unique_ptr<AST>> children;
};
AST * parse(TokenStream & ts);

class ASTVisitor{
	typedef void visit_t(std::unique_ptr<AST> & tree, AST * parent);
	virtual visit_t visitTop = 0;
	virtual visit_t visitAssignment = 0;
	virtual visit_t visitIdentifier = 0;
	virtual visit_t visitReal = 0;
	virtual visit_t visitInteger = 0;
	virtual visit_t visitBoolean = 0;
	virtual visit_t visitString = 0;
	virtual visit_t visitList = 0;
	virtual visit_t visitError = 0;
	virtual visit_t visitNone = 0;
public:
	void visit(std::unique_ptr<AST> & tree, AST * parent)
	{
		switch(tree->type){
			case AST::Type::TOP:
				visitTop(tree, parent);
				break;
			
			case AST::Type::ASSIGNMENT:
				visitAssignment(tree, parent);
				break;
			
			case AST::Type::IDENTIFIER:
				visitIdentifier(tree, parent);
				break;
			
			case AST::Type::REAL:
				visitReal(tree, parent);
				break;
			case AST::Type::BOOLEAN:
				visitBoolean(tree, parent);
				break;
			
			case AST::Type::INTEGER:
				visitInteger(tree, parent);
				break;
			
			case AST::Type::STRING:
				visitString(tree, parent);
				break;
			
			case AST::Type::LIST:
				visitList(tree, parent);
				break;
			
			case AST::Type::ERROR:
				visitError(tree);
				break;
			
			case AST::Type::NONE:
				visitNone(tree);
				break;
		}
	}
};
#endif
